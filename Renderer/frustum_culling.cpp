#include "frustum_culling.h"
#include "debug_renderer.h"


namespace end
{
	void calculate_frustum(frustum_t& frustum, const float4x4& view)
	{
		float fov = 60.0f * (3.14159265359f / 180.0f);			//Vertical Field of View
		float viewWidth = 1280.0f;
		float viewHeight = 800.0f;
		float viewRatio = viewWidth / viewHeight;					//ViewWidth / ViewHeight
		float nearDist = 0.25f;								//Distance from camera to near plane
		float farDist = 5.0f;								//Distance from camera to far plane
		float Hnear = 2.0f * tan(fov / 2.0f) * nearDist;
		float Hfar = 2.0f * tan(fov / 2.0f) * farDist;
		float Wnear = Hnear * viewRatio;
		float Wfar = Hfar * viewRatio;
		float3 nC = view[3].xyz;							//Near Center
		float3 fC = nC;										//Far Center
		nC += view[2].xyz * nearDist;
		fC += view[2].xyz * farDist;
		//debug_renderer::add_line(nC, fC, {0.1f, 1.0f, 0.1f, 1.0f});

		float3 ftl = fC + view[1].xyz * (Hfar * 0.5f) - view[0].xyz * (Wfar * 0.5f);
		float3 ftr = fC + view[1].xyz * (Hfar * 0.5f) + view[0].xyz * (Wfar * 0.5f);
		float3 fbl = fC - view[1].xyz * (Hfar * 0.5f) - view[0].xyz * (Wfar * 0.5f);
		float3 fbr = fC - view[1].xyz * (Hfar * 0.5f) + view[0].xyz * (Wfar * 0.5f);
		float3 ntl = nC + view[1].xyz * (Hnear * 0.5f) - view[0].xyz * (Wnear * 0.5f);
		float3 ntr = nC + view[1].xyz * (Hnear * 0.5f) + view[0].xyz * (Wnear * 0.5f);
		float3 nbl = nC - view[1].xyz * (Hnear * 0.5f) - view[0].xyz * (Wnear * 0.5f);
		float3 nbr = nC - view[1].xyz * (Hnear * 0.5f) + view[0].xyz * (Wnear * 0.5f);
		debug_renderer::add_line(ftl, ftr, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(ftl, fbl, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(fbl, fbr, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(fbr, ftr, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(ftl, ntl, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(ftr, ntr, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(fbl, nbl, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(fbr, nbr, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(ntl, ntr, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(ntl, nbl, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(nbl, nbr, {0.1f, 1.0f, 0.1f, 1.0f});
		debug_renderer::add_line(nbr, ntr, {0.1f, 1.0f, 0.1f, 1.0f});
		frustum[0] = calculate_plane(ftl, ftr, fbl);
		frustum[1] = calculate_plane(ftl, fbl, nbl);
		frustum[2] = calculate_plane(fbl, fbr, nbr);
		frustum[3] = calculate_plane(fbr, ftr, ntr);
		frustum[4] = calculate_plane(ftr, ftl, ntl);
		frustum[5] = calculate_plane(ntl, nbl, nbr);
		float3 centerPoints[6];
		centerPoints[0] = (ftr + ftl + fbl + fbr) / 4;
		debug_renderer::add_line(centerPoints[0], centerPoints[0] + frustum[0].normal, {0.1f, 0.1f, 1.0, 1.0f});
		centerPoints[1] = (ftl + fbl + nbl + ntl) / 4;
		debug_renderer::add_line(centerPoints[1], centerPoints[1] + frustum[1].normal, {0.1f, 0.1f, 1.0, 1.0f});
		centerPoints[2] = (fbl + fbr + nbr + nbl) / 4;
		debug_renderer::add_line(centerPoints[2], centerPoints[2] + frustum[2].normal, {0.1f, 0.1f, 1.0, 1.0f});
		centerPoints[3] = (fbr + ftr + ntr + nbr) / 4;
		debug_renderer::add_line(centerPoints[3], centerPoints[3] + frustum[3].normal, {0.1f, 0.1f, 1.0, 1.0f});
		centerPoints[4] = (ftr + ftl + ntl + ntr) / 4;
		debug_renderer::add_line(centerPoints[4], centerPoints[4] + frustum[4].normal, {0.1f, 0.1f, 1.0, 1.0f});
		centerPoints[5] = (ntl + nbl + nbr + ntr) / 4;
		debug_renderer::add_line(centerPoints[5], centerPoints[5] + frustum[5].normal, {0.1f, 0.1f, 1.0, 1.0f});
	}
	plane_t calculate_plane(float3 a, float3 b, float3 c)
	{
		plane_t returnPlane;
		float3 mather;
		mather = mather.cross((b - a), (c - b));
		mather = mather.normalize(mather);
		returnPlane.normal = mather;
		returnPlane.offset = mather.dot(returnPlane.normal, a);
		return returnPlane;
	}
	int classify_sphere_to_plane(const sphere_t& sphere, const plane_t& plane)
	{
		float3 mather;
		const float mathDone = mather.dot(sphere.center, plane.normal);
		if (mathDone - plane.offset > sphere.radius) return 1;
		if (mathDone - plane.offset < -sphere.radius) return -1;
		return 0;
	}
	int classify_aabb_to_plane(const aabb_t& aabb, const plane_t& plane)
	{
		sphere_t sphere;
		sphere.center = aabb.center;// *2.0f;
		sphere.radius = absDot(aabb.extents, plane.normal);
		return classify_sphere_to_plane(sphere, plane);
	}
	bool aabb_to_frustum(const aabb_t& aabb, const frustum_t& frustum)
	{
		bool returner = true;
		for (const auto& i : frustum)
		{
			const int test = classify_aabb_to_plane(aabb, i);
			if (test < 0) return false;
		}
		return returner;
	}
	float absDot(float3 a, float3 b)
	{
		return (a.x * abs(b.x)) + (a.y * abs(b.y)) + (a.z * abs(b.z));
	}
}
