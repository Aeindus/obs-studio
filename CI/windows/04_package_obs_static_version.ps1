Param(
    [Switch]$Help = $(if (Test-Path variable:Help) { $Help }),
    [Switch]$Quiet = $(if (Test-Path variable:Quiet) { $Quiet }),
    [Switch]$Verbose = $(if (Test-Path variable:Verbose) { $Verbose }),
    [Switch]$BuildInstaller = $(if ($BuildInstaller.isPresent) { $BuildInstaller }),
    [Switch]$CombinedArchs = $(if ($CombinedArchs.isPresent) { $CombinedArchs }),
    [String]$BuildDirectory = $(if (Test-Path variable:BuildDirectory) { "${BuildDirectory}" } else { "build" }),
    [ValidateSet('x86', 'x64')]
    [String]$BuildArch = $(if (Test-Path variable:BuildArch) { "${BuildArch}" } else { ('x86', 'x64')[[System.Environment]::Is64BitOperatingSystem] }),
    [ValidateSet("Release", "RelWithDebInfo", "MinSizeRel", "Debug")]
    [String]$BuildConfiguration = $(if (Test-Path variable:BuildConfiguration) { "${BuildConfiguration}" } else { "RelWithDebInfo" }),
	[String]$Version = $(if (Test-Path variable:Version) { "${Version}" } else { "30.0.0" })
)

##############################################################################
# Windows OBS package function
##############################################################################
#
# This script file can be included in build scripts for Windows or run
# directly
#
##############################################################################


<# 
One big problem with 03_package_obs.ps1 is that in the powershell script it uses the latest
pushed tag which fails because I don't use/want to push tags.
 - The powershell script uses the tag name to set the output file's name.
		While fetching the tags works, git describe --tags --abbrev=0 does not
		Seems to have something to do with the HEAD and the fact that in a workflow
		the head is detached so there's no recent tag to fetch
 - Then there's the cmake file VersionConfig which can use multiple sources for the version.
		This cmake is the most important because it configures all the other source files with the
		proper versions.
		Sources used: RELEASE_CANDIDATE, OBS_VERSION_OVERRIDE, BETA, .git folder
		The errors I get in workflow happen with the .git method. This too tries to get the version from the tag.
		The .git folder exists so it takes that path but the tag taken is "" and so it fails downstream. 

My solution is to customize this package script and use OBS_VERSION_OVERRIDE.
#>

$ErrorActionPreference = "Stop"

function Package-OBS {
    Param(
        [String]$BuildDirectory = $(if (Test-Path variable:BuildDirectory) { "${BuildDirectory}" }),
        [String]$BuildArch = $(if (Test-Path variable:BuildArch) { "${BuildArch}" }),
        [String]$BuildConfiguration = $(if (Test-Path variable:BuildConfiguration) { "${BuildConfiguration}" })
    )

    Write-Status "Package plugin ${ProductName}"
    Ensure-Directory ${CheckoutDir}

    if ($CombinedArchs.isPresent) {
        if (!(Test-Path env:obsInstallerTempDir)) {
            $Env:obsInstallerTempDir = "${CheckoutDir}/install_temp"
        }

        if (!(Test-Path ${CheckoutDir}/install_temp/bin/64bit)) {
            Write-Step "Build 64-bit OBS..."
            Invoke-Expression "cmake -S . -B `"${BuildDirectory}64`" -DCOPIED_DEPENDENCIES=OFF -DCOPY_DEPENDENCIES=ON"
            Invoke-Expression "cmake --build `"${BuildDirectory}64`" --config `"${BuildConfiguration}`""
        }

        if (!(Test-Path ${CheckoutDir}/install_temp/bin/32bit)) {
            Write-Step "Build 32-bit OBS..."
            Invoke-Expression "cmake -S . -B `"${BuildDirectory}32`" -DCOPIED_DEPENDENCIES=OFF -DCOPY_DEPENDENCIES=ON"
            Invoke-Expression "cmake --build `"${BuildDirectory}32`" --config `"${BuildConfiguration}`""
        }

        Write-Step "Prepare Installer run..."
        Invoke-Expression "cmake -S . -B build -DOBS_VERSION_OVERRIDE=`"${Version}`" -DINSTALLER_RUN=ON -DCMAKE_INSTALL_PREFIX=`"${CheckoutDir}/build/install`""
        Write-Step "Execute Installer run..."
        Invoke-Expression "cmake --build build --config `"${BuildConfiguration}`" -t install"

        $CompressVars = @{
            Path = "${CheckoutDir}/build/install/*"
            CompressionLevel = "Optimal"
            DestinationPath = "${FileName}-Win-x86+x64.zip"
        }

        Write-Step "Creating zip archive..."

        $ProgressPreference = $(if ($Quiet.isPresent) { 'SilentlyContinue' } else { 'Continue' })
        Compress-Archive -Force @CompressVars
        $ProgressPreference = 'Continue'

    } elseif ($BuildArch -eq "x64") {
        Write-Step "Install 64-bit OBS..."
        Invoke-Expression "cmake --build `"${BuildDirectory}64`" --config ${BuildConfiguration} -t install"

        $CompressVars = @{
            Path = "${CheckoutDir}/build64/install/bin", "${CheckoutDir}/build64/install/data", "${CheckoutDir}/build64/install/obs-plugins"
            CompressionLevel = "Optimal"
            DestinationPath = "${FileName}-Win-x64.zip"
        }

        Write-Step "Creating zip archive..."

        $ProgressPreference = $(if ($Quiet.isPresent) { 'SilentlyContinue' } else { 'Continue' })
        Compress-Archive -Force @CompressVars
        $ProgressPreference = 'Continue'

    } elseif ($BuildArch -eq "x86") {
        Write-Step "Install 32-bit OBS..."
        Invoke-Expression "cmake --build `"${BuildDirectory}32`" --config ${BuildConfiguration} -t install"

        $CompressVars = @{
            Path = "${CheckoutDir}/build32/install/bin", "${CheckoutDir}/build32/install/data", "${CheckoutDir}/build32/install/obs-plugins"
            CompressionLevel = "Optimal"
            DestinationPath = "${FileName}-Win-x86.zip"
        }

        Write-Step "Creating zip archive..."

        $ProgressPreference = $(if ($Quiet.isPresent) { 'SilentlyContinue' } else { 'Continue' })
        Compress-Archive -Force @CompressVars
        $ProgressPreference = 'Continue'

    }
}

function Package-OBS-Standalone {
    $ProductName = "OBS-Studio"
    $CheckoutDir = Resolve-Path -Path "$PSScriptRoot\..\.."

    . ${CheckoutDir}/CI/include/build_support_windows.ps1

    Write-Step "Fetch OBS tags..."
    $null = git fetch origin --tags

    Ensure-Directory ${CheckoutDir}
    $GitBranch = git rev-parse --abbrev-ref HEAD
    $GitHash = git rev-parse --short=9 HEAD
    $ErrorActionPreference = "SilentlyContinue"
    $GitTag = $Version  	#git describe --tags --abbrev=0
    $ErrorActionPreference = "Stop"

    if(Test-Path variable:BUILD_FOR_DISTRIBUTION) {
        $VersionString = "${GitTag}"
    } else {
        $VersionString = "${GitTag}-${GitHash}"
    }

    $FileName = "${ProductName}-${VersionString}"

    Package-OBS
}

function Print-Usage {
    $Lines = @(
        "Usage: ${_ScriptName}",
        "-Help                    : Print this help",
        "-Quiet                   : Suppress most build process output",
        "-Verbose                 : Enable more verbose build process output",
        "-CombinedArchs           : Create combined architecture package",
        "-BuildDirectory          : Directory to use for builds - Default: build64 on 64-bit systems, build32 on 32-bit systems",
        "-BuildArch               : Build architecture to use (x86 or x64) - Default: local architecture",
        "-BuildConfiguration      : Build configuration to use - Default: RelWithDebInfo",
		"-Version				  : Sets OBS version - Default: 30.0.0"
    )

    $Lines | Write-Host
}


if(!(Test-Path variable:_RunObsBuildScript)) {
    $_ScriptName = "$($MyInvocation.MyCommand.Name)"
    if($Help.isPresent) {
        Print-Usage
        exit 0
    }

    Package-OBS-Standalone
}
