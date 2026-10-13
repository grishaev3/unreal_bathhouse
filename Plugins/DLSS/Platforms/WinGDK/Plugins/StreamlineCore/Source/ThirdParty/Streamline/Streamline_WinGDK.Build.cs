/*
* Copyright (c) 2022 - 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/
using UnrealBuildTool;

public class Streamline_WinGDK : Streamline
{
	protected override string GetPlatformDir(ReadOnlyTargetRules Target)
	{
		return "Win64";
	}

	public Streamline_WinGDK(ReadOnlyTargetRules Target) : base(Target) { }
}
