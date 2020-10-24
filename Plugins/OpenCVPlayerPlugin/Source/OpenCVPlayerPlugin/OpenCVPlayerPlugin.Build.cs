// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnrealBuildTool;


namespace UnrealBuildTool.Rules
{
    public class OpenCVPlayerPlugin : ModuleRules
    {
        public OpenCVPlayerPlugin(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicIncludePaths.AddRange(
                new string[] {
                    "OpenCVPlayerPlugin/Public",
					// ... add public include paths required here ...
				}
                );

            PrivateIncludePaths.AddRange(
                new string[] {
                    "OpenCVPlayerPlugin/Private",
					// ... add other private include paths required here ...
				}
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
					// ... add other public dependencies that you statically link with here ...
				}
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "RHI",
                    "RenderCore",
                    "ShaderCore",
                    "Slate",
                    "SlateCore",
					// ... add private dependencies that you statically link with here ...
				}
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
					// ... add any modules that your module loads dynamically here ...
				}
                );

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
                PublicLibraryPaths.Add(LibDirectory);
                PublicAdditionalLibraries.AddRange(LibModules);
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
                PublicAdditionalLibraries.AddRange(LinuxStdLibs);
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

                List<string> Libs = new List<string>()
                {
                    "opencv_world" + OpenCVVersion + ".lib"
                };
                PublicLibraryPaths.Add(LibDirectory);
                PublicAdditionalLibraries.AddRange(Libs);

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
            else if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                List<string> LibModules = new List<string>()
                {
                    "opencv_videoio",
                    "opencv_imgproc",
                    "opencv_imgcodecs",
                    "opencv_core"
                };
                LibDirectory = "/usr/local/opt/opencv/lib";
                foreach (string s in LibModules)
                {
                    PublicAdditionalLibraries.Add(Path.Combine(LibDirectory, "lib" + s + "." +
                        OpenCVMajorVersion + "." + OpenCVMinorVersion + "." + OpenCVRevisionVersion + ".dylib"));
                }
            }
        }
    }
}
