#include "debug_renderer.h"
#include <array>


// Anonymous namespace
namespace
{
	// Declarations in an anonymous namespace are global BUT only have internal linkage.
	// In other words, these variables are global but are only visible in this source file.

	// Maximum number of debug lines at one time (i.e: Capacity)
	constexpr size_t MAX_LINE_VERTS = 81920; 

	// CPU-side buffer of debug-line verts
	// Copied to the GPU and reset every frame.
	size_t line_vert_count = 0;
	std::array< end::colored_vertex, MAX_LINE_VERTS> line_verts;
}

namespace end
{
	namespace debug_renderer
	{
		//TODO Once you finish thisfile correctly, you should see a green checkmark when you run the game.
		void add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b)
		{
			//TODO Add points to debug_verts, increments debug_vert_count
			line_verts[line_vert_count] = colored_vertex(point_a, color_a);
			line_vert_count += 1;
			line_verts[line_vert_count] = colored_vertex(point_b, color_b);
			line_vert_count += 1;
		}

		void coloredAABB(aabb_t aabb, float4 color)
		{
			float3 ntl = {aabb.center.x - aabb.extents.x, aabb.center.y + aabb.extents.y, aabb.center.z + aabb.extents.z};
			float3 ntr = ntl;
			ntr.x += aabb.extents.x * 2;
			float3 nbl = ntl;
			nbl.y -= aabb.extents.y * 2;
			float3 nbr = nbl;
			nbr.x += aabb.extents.x * 2;
			float3 btl = ntl;
			btl.z -= aabb.extents.z * 2;
			float3 btr = ntr;
			btr.z -= aabb.extents.z * 2;
			float3 bbl = nbl;
			bbl.z -= aabb.extents.z * 2;
			float3 bbr = nbr;
			bbr.z -= aabb.extents.z * 2;
			//front
			add_line(ntr, ntl, color);
			add_line(ntl, nbl, color);
			add_line(nbl, nbr, color);
			add_line(ntr, nbr, color);
			//sides
			add_line(ntl, btl, color);
			add_line(nbl, bbl, color);
			add_line(nbr, bbr, color);
			add_line(ntr, btr, color);
			//back
			add_line(btl, btr, color);
			add_line(btl, bbl, color);
			add_line(bbl, bbr, color);
			add_line(btr, bbr, color);
		}

		void clear_lines()
		{
			//TODO Resets debug_vert_count to 0
			line_vert_count = 0;
		}

		const colored_vertex* get_line_verts()
		{ 
			//TODO Returns the line vert array pointer
			return line_verts.data();
		}

		size_t get_line_vert_count() 
		{ 
			//TODO Returns how many vertices there are now
			return line_vert_count;
		}

		size_t get_line_vert_capacity()
		{
			//TODO returns the maximum size the line vert array
			return line_verts.size();
		}
	}
}