// ReSharper disable CppInconsistentNaming
#include "dev_app.h"
#include "math_types.h"
#include "debug_renderer.h"
#include "pools.h"
#include "renderer.h"
#include <iostream>
//#include <WinUser.h>
#include <DirectXMath.h>
//TODO include debug_renderer.h and pools.h and anything else you might need here

//extern end::float4x4 lookAt, turnTo, mouseLook;
namespace end
{

	struct particle
	{
		float3 pos;
		float3 prev_pos;
		float4 color;
		float lifeTime;
		float3 velocity;
	};

	struct sortedEmitter
	{
		sorted_pool_t<particle, 4096> sorted_particles;
	};

	pool_t<particle, 4096> shared_pool;

	struct emitter
	{
		float3 spawn_pos;
		float4 spawn_color;
		// indices into the shared pool
		sorted_pool_t<int16_t, 1024> indices;
	};

	double delta_time = 0.0f;
	float4 c = {0.3f, 0.0f, 0.0f, 1};

	bool MaxColor = false;

	sortedEmitter sortedPool;
	emitter emOne;
	emitter emTwo;
	emitter emThree;
	emitter emFour;
	float3 gravity = {0, -9.8f, 0};
	/// <summary>
	/// Moving Target Position
	/// </summary>
	float3 mTPOS = {0,0.5f,0};
	/// <summary>
	/// Look at Object Position
	/// </summary>
	float3 lAPOS = {-2.5f, 5.0f, -7.5f};
	/// <summary>
	/// Turn too Object Position
	/// </summary>
	float3 tTPOS = {2.5f, 5.0f, 2.5f};
	/// <summary>
	/// mouse Look Position
	/// </summary>
	float3 mLPOS = {0.0f, 10.0f, -25.0f};
	double dev_app_t::get_delta_time()const
	{
		return delta_time;
	}

	dev_app_t::dev_app_t()
	{
		std::cout << "Log whatever you need here.\n"; // Don’t forget to include <iostream>
	}

	double calc_delta_time()
	{
		static std::chrono::time_point<std::chrono::high_resolution_clock> last_time = std::chrono::high_resolution_clock::now();

		std::chrono::time_point<std::chrono::high_resolution_clock> new_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds = new_time - last_time;
		last_time = new_time;

		return std::min(1.0 / 15.0, elapsed_seconds.count());
	}
	
	void dev_app_t::update()
	{
		delta_time = calc_delta_time();
#pragma region Frustum Stuff
		frustum_t frustum;
		calculate_frustum(frustum, movingTarget);
		aabb_t aabbs[3];
		aabbs[0].center = {0.0f, 0.0f, 0.0f};
		aabbs[0].extents = {1.0f, 1.0f, 1.0f};
		if (end::aabb_to_frustum(aabbs[0], frustum) == false) end::debug_renderer::coloredAABB(aabbs[0], {0.1f, 1.0f, 0.1f, 1.0f});
		else end::debug_renderer::coloredAABB(aabbs[0], {1.0f, 0.1f, 0.1f, 1.0f});
		aabbs[1].center = {-3.0f, 2.0f, -5.0f};
		aabbs[1].extents = {3.0f, 2.0f, 1.0f};
		if (end::aabb_to_frustum(aabbs[1], frustum) == false) end::debug_renderer::coloredAABB(aabbs[1], {0.75f, 0.4f, 0.3f, 1.0f});
		else end::debug_renderer::coloredAABB(aabbs[1], {1.0f, 0.1f, 0.1f, 1.0f});
		aabbs[2].center = {4.0f, 3.0f, 5.0f};
		aabbs[2].extents = {8.0f, 3.0f, 1.0f};
		if (end::aabb_to_frustum(aabbs[2], frustum) == false) end::debug_renderer::coloredAABB(aabbs[2], {0.0f, 0.75f, 0.5f, 1.0f});
		else end::debug_renderer::coloredAABB(aabbs[2], {1.0f, 0.1f, 0.1f, 1.0f});
#pragma endregion

#pragma region Movement Key Control
		movingTarget[3].xyz = mTPOS;
		drawDebug(movingTarget);
		lookAt[3].xyz = lAPOS;
		drawDebug(lookAt);
		turnTo[3].xyz = tTPOS;
		drawDebug(turnTo);
		float moveSpeed = 2.5f * delta_time;
		//W key
		if (bitsetter[0x57] == true) mLPOS += (*mouseLook)[2].xyz * (moveSpeed * 2);
		//S key
		if (bitsetter[0x53] == true) mLPOS -= (*mouseLook)[2].xyz * (moveSpeed * 2);
		//D key
		if (bitsetter[0x44] == true) mLPOS += (*mouseLook)[0].xyz * (moveSpeed * 2);
		//A key
		if (bitsetter[0x41] == true) mLPOS -= (*mouseLook)[0].xyz * (moveSpeed * 2);
		// Right arrow
		if (bitsetter[0x27] == true)
		{
			DirectX::XMMATRIX rotateMe = reinterpret_cast<DirectX::FXMMATRIX&>(movingTarget);
			DirectX::XMMATRIX posRot = DirectX::XMMatrixRotationY(2.5f * delta_time);
			DirectX::XMMATRIX rotator = DirectX::XMMatrixMultiply(posRot, rotateMe);
			movingTarget = reinterpret_cast<float4x4&>(rotator);
		}
		// Left arrow
		if (bitsetter[0x25] == true)
		{
			DirectX::XMMATRIX rotateMe = reinterpret_cast<DirectX::FXMMATRIX&>(movingTarget);
			DirectX::XMMATRIX negRot = DirectX::XMMatrixRotationY(-2.5f * delta_time);
			DirectX::XMMATRIX rotator = DirectX::XMMatrixMultiply(negRot, rotateMe);
			movingTarget = reinterpret_cast<float4x4&>(rotator);
		}
		// Up arrow
		if (bitsetter[0x26] == true) mTPOS += movingTarget[2].xyz * (moveSpeed);
		// Down arrow
		if (bitsetter[0x28] == true) mTPOS -= movingTarget[2].xyz * (moveSpeed);
		// Update mouse look position
		(*mouseLook)[3].xyz = mLPOS;
		//if(bitsetter[0x02] == true)

		deltaX = mouseX - mousePrevX;
		deltaY = mouseY - mousePrevY;
		mousePrevX = mouseX;
		mousePrevY = mouseY;
		(DirectX::XMMATRIX&)(*mouseLook)[0].xyz = DirectX::XMMatrixMultiply((DirectX::FXMMATRIX&)(*mouseLook)[0].xyz, DirectX::XMMatrixRotationY(deltaX * delta_time * 2.0f));
		(DirectX::XMMATRIX&)(*mouseLook)[0].xyz = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(deltaY * delta_time * 2.0f), (DirectX::FXMMATRIX&)(*mouseLook)[0].xyz);

		turnToTarget(turnTo, movingTarget);
		lookAtTarget(lookAt, movingTarget);
#pragma endregion

#pragma region Grid Lines
		//This drawn the green checkmark
		//end::debug_renderer::add_line(float3(-2, 0, 0), float3(0, -3, 0), float4(0.1f, 1, 0.1f, 1));
		//end::debug_renderer::add_line(float3(0, -3, 0), float3(3, 4, 0), float4(0.1f, 1, 0.1f, 1));
		float3 a = float3();
		float3 b = float3();
		a.x = -10.0f;
		a.y = 0.0f;
		a.z = -10.0f;
		b.x = -10.0f;
		b.y = 0.0f;
		b.z = 10.0f;

		//TODO do you Updates here
		for (int i = 0; i < 41; ++i)
		{
			end::debug_renderer::add_line(a, b, c);
			a.x += 0.5f;
			b.x += 0.5f;
		}
		a.x = -10.0f;
		a.z = 10.0f;
		b.x = 10.0f;
		for (int i = 0; i < 41; ++i)
		{
			end::debug_renderer::add_line(a, b, c);
			a.z -= 0.5f;
			b.z -= 0.5f;
		}
#pragma endregion

#pragma region Grid Color
		if (MaxColor == false)
		{
			c.xyz += {static_cast<float>(0.2f * delta_time), static_cast<float>(0.0f * delta_time), static_cast<float>(0.0f * delta_time)};
			if (c.x >= 0.7f) MaxColor = true;
		}
		if (MaxColor == true)
		{
			c.xyz -= {static_cast<float>(0.2f * delta_time), static_cast<float>(0.0f * delta_time), static_cast<float>(0.0f * delta_time)};
			if (c.x <= 0.3f) MaxColor = false;
		}
#pragma endregion

		//Insert();
#pragma region Free Emitters
		emOne.spawn_pos = {-10.0f, 0.0f, -10.0f};
		emOne.spawn_color = makeRandColor(0.1f, 0.75f);
		freeEmitter(emOne);
		emitterMaker(emOne);
		emTwo.spawn_pos = {-10.f, 0, 10.f};
		emTwo.spawn_color = makeRandColor(0.1f, 0.75f);
		freeEmitter(emTwo);
		emitterMaker(emTwo);
		emThree.spawn_pos = {10.0f, 0, -10.0f};
		emThree.spawn_color = makeRandColor(0.1f, 0.75f);
		freeEmitter(emThree);
		emitterMaker(emThree);
		emFour.spawn_pos = {10.0f, 0, 10.0f};
		emFour.spawn_color = makeRandColor(0.1f, 0.75f);
		freeEmitter(emFour);
		emitterMaker(emFour);
#pragma endregion

#pragma region Sorted Emission
		for (int16_t i = 0; i < sortedPool.sorted_particles.size(); ++i)
		{
			if (sortedPool.sorted_particles[i].lifeTime > 0)
			{
				Physics(sortedPool.sorted_particles[i]);
				sortedPool.sorted_particles[i].lifeTime -= delta_time;
			}
			if (sortedPool.sorted_particles[i].lifeTime <= 0)
			{
				sortedPool.sorted_particles.free(i--);
			}
			end::debug_renderer::add_line(sortedPool.sorted_particles[i].pos, sortedPool.sorted_particles[i].prev_pos, sortedPool.sorted_particles[i].color);
		}
	}
#pragma endregion

	/// <summary>
	/// Used to insert particles into the sorted pool.
	/// </summary>
	void dev_app_t::Insert()
	{
		for (int16_t i = 0; i < 2; ++i)
		{
			int16_t index = sortedPool.sorted_particles.alloc();
			if (index == -1) break;
			particle newParticle;
			newParticle.pos = {0, 0.0f, 0.0f,};
			//newParticle.color = {0.1f, 1.0f, 0.1f, 1.0f};
			newParticle.color = makeRandColor(0.1f, 0.75f);
			newParticle.lifeTime = makeARand(1.5f, 2.5f);
			newParticle.velocity = {makeARand(-2.5f, 2.5f), 10.75f, makeARand(-2.5f, 2.5f)};
			sortedPool.sorted_particles[index] = newParticle;
		}
	}

	/// <summary>
	/// Inserts particles into Emitter
	/// </summary>
	void dev_app_t::freeEmitter(emitter& freeEmitter)
	{
		for (int16_t i = 0; i < 1; ++i)
		{
			int16_t index = shared_pool.alloc();
			if (index == -1) break;
			int16_t indicIndex = freeEmitter.indices.alloc();
			if (indicIndex == -1)
			{
				shared_pool.free(index);
				break;
			}
			freeEmitter.indices[indicIndex] = index;
			particle newParticle;
			newParticle.pos = freeEmitter.spawn_pos;
			//newParticle.color = {0.1f, 1.0f, 0.1f, 1.0f};
			newParticle.color = freeEmitter.spawn_color;
			newParticle.lifeTime = makeARand(1.5f, 2.5f);
			newParticle.velocity = {makeARand(-2.5f, 2.5f), 10.75f, makeARand(-2.5f, 2.5f)};
			shared_pool[index] = newParticle;
		}
	}

	/// <summary>
	/// Emits particles at provided emitter's position with color.
	/// </summary>
	void dev_app_t::emitterMaker(emitter& emitter)
	{
		for (int16_t i = 0; i < emitter.indices.size(); ++i)
		{
			particle& p = shared_pool[emitter.indices[i]];
			if (p.lifeTime > 0)
			{
				Physics(p);
				p.lifeTime -= delta_time;
			}
			if (p.lifeTime <= 0)
			{
				shared_pool.free(emitter.indices[i]);
				emitter.indices.free(i--);
			}
		}

		for (int16_t i = 0; i < emitter.indices.size(); ++i)
		{

			particle& p = shared_pool[emitter.indices[i]];
			end::debug_renderer::add_line(p.pos, p.prev_pos, p.color);
		}
	}

	/// <summary>
	/// Applies gravity physics to a particle
	/// </summary>
	void dev_app_t::Physics(particle& part)
	{
		part.prev_pos = part.pos;
		part.pos += part.velocity * static_cast<float>(delta_time);
		part.velocity += gravity * static_cast<float>(delta_time);
	}
	/// <summary>
	/// Makes a random float in range of min to max
	/// </summary>
	float dev_app_t::makeARand(float min, float max)
	{
		float returnIt = (static_cast<float>(rand()) / RAND_MAX) * (max - min) + min;
		return returnIt;
	}
	/// <summary>
	/// Returns a random float4 for a color, with w always being 1.0f
	/// </summary>
	float4 dev_app_t::makeRandColor(float min, float max)
	{
		float4 returnColor;
		returnColor.x = (static_cast<float>(rand()) / RAND_MAX) * (max - min) + min;
		if (returnColor.x >= max - (min * 2)) return returnColor;
		returnColor.y = (static_cast<float>(rand()) / RAND_MAX) * (max - min) + min;
		if (returnColor.y >= max - (min * 2)) return returnColor;
		returnColor.z = (static_cast<float>(rand()) / RAND_MAX) * (max - min) + min;
		if (returnColor.z >= max - (min * 2)) return returnColor;
		returnColor.w = 1.0f;
		return returnColor;
	}
	/// <summary>
	/// Draw debug lines at given matrix position
	/// </summary>
	void dev_app_t::drawDebug(float4x4& matrix)
	{
		float3 startPos = matrix[3].xyz;
		float3 length = {1.0f, 1.0f, 1.0f};
		end::debug_renderer::add_line(startPos, startPos + matrix[2].xyz * length, {0, 0, 1.0f, 1.0f});
		end::debug_renderer::add_line(startPos, startPos + matrix[0].xyz * length, {1.0f, 0, 0, 1.0f});
		end::debug_renderer::add_line(startPos, startPos + matrix[1].xyz * length, {0, 1.0f, 0, 1.0f});
	}
	void dev_app_t::turnToTarget(float4x4& turner, float4x4 target)
	{
		float3 vec = turner[3].xyz - target[3].xyz;
		float dotterY = vec.dot(vec, turner[0].xyz);
		float dotterX = vec.dot(vec, turner[1].xyz);
		if(dotterY > 0)
		{
			DirectX::XMMATRIX rotateMe = reinterpret_cast<DirectX::FXMMATRIX&>(turner);
			DirectX::XMMATRIX negRot = DirectX::XMMatrixRotationY(-0.5f * delta_time);
			DirectX::XMMATRIX rotator = DirectX::XMMatrixMultiply(negRot, rotateMe);
			turner = reinterpret_cast<float4x4&>(rotator);
		}
		
		if (dotterY < 0)
		{
			DirectX::XMMATRIX rotateMe = reinterpret_cast<DirectX::FXMMATRIX&>(turner);
			DirectX::XMMATRIX posRot = DirectX::XMMatrixRotationY(0.5f * delta_time);
			DirectX::XMMATRIX rotator = DirectX::XMMatrixMultiply(posRot, rotateMe);
			turner = reinterpret_cast<float4x4&>(rotator);
		}
		if (dotterX > 0)
		{
			DirectX::XMMATRIX rotateMe = reinterpret_cast<DirectX::FXMMATRIX&>(turner);
			DirectX::XMMATRIX negRot = DirectX::XMMatrixRotationX(0.5f * delta_time);
			DirectX::XMMATRIX rotator = DirectX::XMMatrixMultiply(negRot, rotateMe);
			turner = reinterpret_cast<float4x4&>(rotator);
		}
		if (dotterX < 0)
		{
			DirectX::XMMATRIX rotateMe = reinterpret_cast<DirectX::FXMMATRIX&>(turner);
			DirectX::XMMATRIX posRot = DirectX::XMMatrixRotationX(-0.5f * delta_time);
			DirectX::XMMATRIX rotator = DirectX::XMMatrixMultiply(posRot, rotateMe);
			turner = reinterpret_cast<float4x4&>(rotator);
		}
		stabilize(turner);
	}
	void dev_app_t::stabilize(float4x4& stabile)
	{
		float4 newZ = stabile[2];
		float3 mathDealie = {0, 1.0f, 0};
		float3 blank;
		float3 newX = blank.cross(mathDealie, newZ.xyz);
		newX = blank.normalize(newX);
		float3 newY = blank.cross(newZ.xyz, newX);
		newY = blank.normalize(newY);
		stabile[0] = {newX.x, newX.y, newX.z, 0};
		stabile[1] = {newY.x, newY.y, newY.z, 0};
		stabile[2] = newZ;
	}
	void dev_app_t::lookAtTarget(float4x4& looker, float4x4 target)
	{
		float3 newZ = target[3].xyz - looker[3].xyz;
		newZ = newZ.normalize(newZ);
		float3 mathDealie = {0, 1.0f, 0};
		float3 blank;
		float3 newX = blank.cross(mathDealie, newZ);
		newX = blank.normalize(newX);
		float3 newY = blank.cross(newZ, newX);
		newY = blank.normalize(newY);
		looker[0] = {newX.x, newX.y, newX.z, 0};
		looker[1] = {newY.x, newY.y, newY.z, 0};
		looker[2] = {newZ.x, newZ.y, newZ.z, 0.0f};
	}

	void dev_app_t::readBinaryFile()
	{
		std::ifstream file;
		file.open("terrain.bin", std::ios::binary);
		file.read(reinterpret_cast<char*>(&vert_count), sizeof(uint32_t));
		std::vector<float3> pos;
		pos.resize(vert_count);
		std::vector<float3> normals;
		normals.resize(vert_count);
		std::vector<float2> uvs;
		uvs.resize(vert_count);
		file.read(reinterpret_cast<char*>(pos.data()), sizeof(float3) * vert_count);
		file.read(reinterpret_cast<char*>(normals.data()), sizeof(float3) * vert_count);
		file.read(reinterpret_cast<char*>(uvs.data()), sizeof(float2) * 2);
		for (int i = 0; i < pos.size(); ++i)
		{
			VERTEX vert = {pos[i], normals[i], uvs[i]};
			vertecies.push_back(vert);
		}
		file.close();
	}
}