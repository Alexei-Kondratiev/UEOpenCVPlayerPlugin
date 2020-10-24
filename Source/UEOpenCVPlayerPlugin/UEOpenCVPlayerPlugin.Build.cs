// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnrealBuildTool;


public class UEOpenCVPlayerPlugin : ModuleRules
{
    public UEOpenCVPlayerPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        const string OpenCVMajorVersion = "3";
        const string OpenCVMinorVersion = "4";
        const string OpenCVRevisionVersion = "1";
        string BaseDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
        string LibDirectory = Path.Combine(BaseDirectory, "Binaries", Target.Platform.ToString());

        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            const string LinuxStdMajorVersion = "1";
            const string LinuxStdMinorVersion = "0";

            List<string> LibModules = new List<string>()
                {
                    "opencv_videoio",
                    "opencv_imgproc",
                    "opencv_imgcodecs",
                    "opencv_core"
                };
            foreach (string s in LibModules)
            {
                RuntimeDependencies.Add(Path.Combine(LibDirectory, "lib" + s + ".so"));
                RuntimeDependencies.Add(Path.Combine(LibDirectory, "lib" + s + ".so." + 
                    OpenCVMajorVersion + "." + OpenCVMinorVersion));
                RuntimeDependencies.Add(Path.Combine(LibDirectory, "lib" + s + ".so." + 
                    OpenCVMajorVersion + "." + OpenCVMinorVersion + "." + OpenCVRevisionVersion));
            }

            List<string> LinuxStdLibs = new List<string>()
                {
                    "c++",
                    "c++abi"
                };
            foreach (string s in LinuxStdLibs)
            {
                RuntimeDependencies.Add(Path.Combine(LibDirectory, "lib" + s + ".so"));
                RuntimeDependencies.Add(Path.Combine(LibDirectory, "lib" + s + ".so." + 
                    LinuxStdMajorVersion));
                RuntimeDependencies.Add(Path.Combine(LibDirectory, "lib" + s + ".so." + 
                    LinuxStdMajorVersion + "." + LinuxStdMinorVersion));
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform.ToString() == "UWP64")
        {
            const string OpenCVVersion = OpenCVMajorVersion + OpenCVMinorVersion + OpenCVRevisionVersion;

            List<string> LibModules = new List<string>()
                {
                    "opencv_world" + OpenCVVersion + ".dll",
                    "opencv_ffmpeg" + OpenCVVersion + "_64.dll"
                };
            foreach (string s in LibModules)
            {
                RuntimeDependencies.Add(Path.Combine(LibDirectory, s));
            }
        }
    }
}
