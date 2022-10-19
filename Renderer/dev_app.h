#pragma once
#include <cstdint>
#include <chrono>
#include <bitset>
#include <fstream>
#include <vector>

#include "math_types.h"
#include "pools.h"

namespace end
{
	struct emitter;
	struct particle;


	// Simple app class for development and testing purposes
	struct dev_app_t
	{
		void update();

		dev_app_t();

		double get_delta_time()const;

		void changeColors(float4 color);

		void Insert();

		void Physics(particle& part);

		float makeARand(float min, float max);

		float4 makeRandColor(float min, float max);

		void freeEmitter(emitter& freeEmitter);

		void emitterMaker(emitter& emitter);

		void drawDebug(float4x4& matrix);

		void turnToTarget(float4x4& turner, float4x4 target);

		void stabilize(float4x4& stabile);

		void lookAtTarget(float4x4& looker, float4x4 target);

		std::bitset<256> bitsetter;

		float4x4 lookAt;
		float4x4 turnTo;
		float4x4_a* mouseLook;
		float4x4 movingTarget;
		float mouseX;
		float mouseY;
		float mousePrevX = mouseX;
		float mousePrevY = mouseY;
		float deltaX;
		float deltaY;

		uint32_t vert_count;
		struct VERTEX
		{
			end::float3 pos;
			end::float3 normal;
			end::float2 uv;
		};
		std::vector<VERTEX> vertecies;
		//std::ifstream fileizer;
		void readBinaryFile();
	};

	
}
