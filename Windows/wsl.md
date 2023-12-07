## Step 1 - Enable the Windows Subsystem for Linux
You must first enable the "Windows Subsystem for Linux" optional feature before installing any Linux distributions on Windows.

Open PowerShell as Administrator and run:

PowerShellCopy

dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
We recommend now moving on to step #2, updating to WSL 2, but if you wish to only install WSL 1, you can now restart your machine and move on to Step 6 - Install your Linux distribution of choice. To update to WSL 2, wait to restart your machine and move on to the next step.

## Step 2 - Check requirements for running WSL 2
To update to WSL 2, you must be running Windows 10.

For x64 systems: Version 1903 or higher, with Build 18362 or higher.For ARM64 systems: Version 2004 or higher, with Build 19041 or higher.Builds lower than 18362 do not support WSL 2. Use the Windows Update Assistant to update your version of Windows.
To check your version and build number, select Windows logo key + R, type winver, select OK. Update to the latest Windows version in the Settings menu.

 Note

If you are running Windows 10 version 1903 or 1909, open "Settings" from your Windows menu, navigate to "Update & Security" and select "Check for Updates". Your Build number must be 18362.1049+ or 18363.1049+, with the minor build # over .1049. Read more: WSL 2 Support is coming to Windows 10 Versions 1903 and 1909.

## Step 3 - Enable Virtual Machine feature
Before installing WSL 2, you must enable the Virtual Machine Platform optional feature. Your machine will require virtualization capabilities to use this feature.

Open PowerShell as Administrator and run:

PowerShellCopy

dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
Restart your machine to complete the WSL install and update to WSL 2.

## Step 4 - Download the Linux kernel update package
Download the latest package:

WSL2 Linux kernel update package for x64 machines
 Note

If you're using an ARM64 machine, please download the ARM64 package instead. If you're not sure what kind of machine you have, open Command Prompt or PowerShell and enter: systeminfo | find "System Type". Caveat: On non-English Windows versions, you might have to modify the search text, translating the "System Type" string. You may also need to escape the quotations for the find command. For example, in German systeminfo | find '"Systemtyp"'.

Run the update package downloaded in the previous step. (Double-click to run - you will be prompted for elevated permissions, select ‘yes’ to approve this installation.)

Once the installation is complete, move on to the next step - setting WSL 2 as your default version when installing new Linux distributions. (Skip this step if you want your new Linux installs to be set to WSL 1).

 Note

For more information, read the article changes to updating the WSL2 Linux kernel, available on the Windows Command Line Blog.

## Step 5 - Set WSL 2 as your default version
Open PowerShell and run this command to set WSL 2 as the default version when installing a new Linux distribution:

PowerShellCopy

wsl --set-default-version 2 

## Step 6 - Install your Linux distribution of choice
Open the Microsoft Store and select your favorite Linux distribution.

The following links will open the Microsoft store page for each distribution:

Ubuntu 18.04 LTSUbuntu 20.04 LTSopenSUSE Leap 15.1SUSE Linux Enterprise Server 12 SP5SUSE Linux Enterprise Server 15 SP1Kali LinuxDebian GNU/LinuxFedora Remix for WSLPengwinPengwin EnterpriseAlpine WSLRaft(Free Trial)
From the distribution's page, select "Get".

The first time you launch a newly installed Linux distribution, a console window will open and you'll be asked to wait for a minute or two for files to de-compress and be stored on your PC. All future launches should take less than a second.

You will then need to create a user account and password for your new Linux distribution.

CONGRATULATIONS! You've successfully installed and set up a Linux distribution that is completely integrated with your Windows operating system!

Troubleshooting installation
If you run into an issue during the install process, check the installation section of the troubleshooting guide.

Downloading distributions
There are some scenarios in which you may not be able (or want) to, install WSL Linux distributions using the Microsoft Store. You may be running a Windows Server or Long-Term Servicing (LTSC) desktop OS SKU that doesn't support Microsoft Store, or your corporate network policies and/or admins do not permit Microsoft Store usage in your environment. In these cases, while WSL itself is available, you may need to download Linux distributions directly.

If the Microsoft Store app is not available, you can download and manually install Linux distributions using these links:

Ubuntu 20.04Ubuntu 20.04 ARMUbuntu 18.04Ubuntu 18.04 ARMUbuntu 16.04Debian GNU/LinuxKali LinuxSUSE Linux Enterprise Server 12SUSE Linux Enterprise Server 15 SP2openSUSE Leap 15.2Fedora Remix for WSL
This will cause the <distro>.appx packages to download to a folder of your choosing.

If you prefer, you can also download your preferred distribution(s) via the command line, you can use PowerShell with the Invoke-WebRequest cmdlet. For example, to download Ubuntu 20.04:

PowerShellCopy

Invoke-WebRequest -Uri https://aka.ms/wslubuntu2004 -OutFile Ubuntu.appx -UseBasicParsing
 Tip

If the download is taking a long time, turn off the progress bar by setting $ProgressPreference = 'SilentlyContinue'

You also have the option to use the curl command-line utility for downloading. To download Ubuntu 20.04 with curl:

Ubuntu默认发行版 https://aka.ms/wslubuntu

Ubuntu 22.04 LTS https://aka.ms/wslubuntu2204

Ubuntu 20.04 https://aka.ms/wslubuntu2004

Ubuntu 20.04 ARM https://aka.ms/wslubuntu2004arm

Ubuntu 18.04 https://aka.ms/wsl-ubuntu-1804

Ubuntu 18.04 ARM https://aka.ms/wsl-ubuntu-1804-arm

Ubuntu 16.04 https://aka.ms/wsl-ubuntu-1604

ConsoleCopy

curl.exe -L -o ubuntu-2004.appx https://aka.ms/wsl-ubuntu-2004
In this example, curl.exe is executed (not just curl) to ensure that, in PowerShell, the real curl executable is invoked, not the PowerShell curl alias for Invoke-WebRequest.

Once the distribution has been downloaded, navigate to the folder containing the download and run the following command in that directory, where app-name is the name of the Linux distribution .appx file.

PowershellCopy

Add-AppxPackage .\app_name.appx
If you are using Windows server, or run into problems running the command above you can find the alternate install instructions on the Windows Server documentation page to install the .appx file by changing it to a zip file.

Once your distribution is installed, follow the instructions to create a user account and password for your new Linux distribution.

Install Windows Terminal (optional)
Using Windows Terminal enables you to open multiple tabs or window panes to display and quickly switch between multiple Linux distributions or other command lines (PowerShell, Command Prompt, PowerShell, Azure CLI, etc). You can fully customize your terminal with unique color schemes, font styles, sizes, background images, and custom keyboard shortcuts. Learn more.

Install Windows Terminal.

Recommended content Install Linux Subsystem on Windows Server
Learn how to install the Linux Subsystem on Windows Server. WSL is available for installation on Windows Server 2019 (version 1709) and later.

Troubleshooting Windows Subsystem for Linux
Provides detailed information about common errors and issues people run into while running Linux on the Windows Subsystem for Linux.

Windows Terminal installation
In this quickstart, you will learn how to install and set up Windows Terminal.
