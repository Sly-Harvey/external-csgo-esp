#pragma once
#include <iostream>
#include <windows.h>
#include "types.h"

void ConvertToRange(Vector2& point)
{
	point.X /= 1920.0f;
	point.X *= 2.0f;
	point.X -= 1.0f;

	point.Y /= 1080.0f;
	point.Y *= 2.0f;
	point.Y -= 1.0f;
}

bool WorldToScreen(Vector3 vectorOrigin, Vector2& screenVector, float* matrix)
{
	screenVector.X = vectorOrigin.X * matrix[0] + vectorOrigin.Y * matrix[1] + vectorOrigin.Z * matrix[2] + matrix[3];
	screenVector.Y = vectorOrigin.X * matrix[4] + vectorOrigin.Y * matrix[5] + vectorOrigin.Z * matrix[6] + matrix[7];
	float W = vectorOrigin.X * matrix[12] + vectorOrigin.Y * matrix[13] + vectorOrigin.Z * matrix[14] + matrix[15];

	if (W < 0.0f)
		return false;

	Vector2 NDC;
	NDC.X = screenVector.X / W;
	NDC.Y = screenVector.Y / W;

	screenVector.X = (1920 / 2 * NDC.X) + (NDC.X + 1920 / 2);
	screenVector.Y = (1080 / 2 * NDC.Y) + (NDC.Y + 1080/ 2);

	ConvertToRange(screenVector);

	return true;
}