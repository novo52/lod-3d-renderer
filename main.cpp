/*
OneLoneCoder.com - 3D Graphics Part #4 - Texturing & Depth Buffers
"Tredimensjonal Grafikk" - @Javidx9
License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9
GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE
From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!
Background
~~~~~~~~~~
3D Graphics is an interesting, visually pleasing suite of algorithms. This is the
first video in a series that will demonstrate the fundamentals required to
build your own software based 3D graphics systems.
Video
~~~~~
https://youtu.be/ih20l3pJoeU
https://youtu.be/XgMWc6LumG4
https://youtu.be/HXSuNxpCzdM
https://youtu.be/nBzCS-Y0FcY
Author
~~~~~~
Twitter: @javidx9
Blog: http://www.onelonecoder.com
Discord: https://discord.gg/WhwHUMV
Last Updated: 01/09/2018
*/


// PLEASE NOTE! The video shows the Spyro The Dragon Level - I can't redistribute that
// so this file is configured to show the textured Jario cube. Please ensure you have
// downloaded the "Jario.spr" file from


/* Alterations Joseph21 - 2022-07-19
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 *   - added path for sprite files
 * Port to PixelGameEngine - 2022-07-19
 *   - see cheat sheet for generic adaptations needed for port
 *   - rewrote GetColour() to a version that is compatible with PGE
 *   - changed screen and pixel sizes
 */

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine/olcPixelGameEngine.h"

#include <fstream>
#include <strstream>
#include <algorithm>
#include <string>

#include "mat4x4.cpp"
#include "vec2d.cpp"
#include "vec3d.cpp"
#include "mesh.cpp"

 // Created a 2D structure to hold texture coordinates



class olcEngine3D : public olc::PixelGameEngine
{
public:
	olcEngine3D()
	{
		sAppName = "3D Demo";
	}


private:
	mesh meshCube;
	mat4x4 matProj;	// Matrix that converts from view space to screen space
	vec3d vCamera;	// Location of camera in world space
	vec3d vLookDir;	// Direction vector along the direction camera points
	float fYaw;		// FPS Camera rotation in XZ plane
	float fTheta;	// Spins World transform

	olc::Sprite* sprTex1;

	vec3d Vector_IntersectPlane(vec3d& plane_p, vec3d& plane_n, vec3d& lineStart, vec3d& lineEnd, float& t)
	{
		plane_n.normalize();
		float plane_d = -plane_n.dot(plane_p);
		float ad = lineStart.dot(plane_n);
		float bd = lineEnd.dot(plane_n);
		t = (-plane_d - ad) / (bd - ad);
		vec3d lineStartToEnd = lineEnd - lineStart;
		vec3d lineToIntersect = lineStartToEnd * t;
		return lineStart + lineToIntersect;
	}

	int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
	{
		// Make sure plane normal is indeed normal
		plane_n.normalize();

		// Return signed shortest distance from point to plane, plane normal must be normalised
		auto dist = [&](vec3d& p)
		{
			vec3d n = p.normal();
			return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
		};

		// Create two temporary storage arrays to classify points either side of plane
		// If distance sign is positive, point lies on "inside" of plane
		vec3d* inside_points[3];  int nInsidePointCount = 0;
		vec3d* outside_points[3]; int nOutsidePointCount = 0;
		vec2d* inside_tex[3]; int nInsideTexCount = 0;
		vec2d* outside_tex[3]; int nOutsideTexCount = 0;


		// Get signed distance of each point in triangle to plane
		float d0 = dist(in_tri.p[0]);
		float d1 = dist(in_tri.p[1]);
		float d2 = dist(in_tri.p[2]);

		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; }
		else {
			outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
		}
		if (d1 >= 0) {
			inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
		}
		else {
			outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1];
		}
		if (d2 >= 0) {
			inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
		}
		else {
			outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
		}

		// Now classify triangle points, and break the input triangle into
		// smaller output triangles if required. There are four possible
		// outcomes...

		if (nInsidePointCount == 0)
		{
			// All points lie on the outside of plane, so clip whole triangle
			// It ceases to exist

			return 0; // No returned triangles are valid
		}

		if (nInsidePointCount == 3)
		{
			// All points lie on the inside of plane, so do nothing
			// and allow the triangle to simply pass through
			out_tri1 = in_tri;

			return 1; // Just the one returned original triangle is valid
		}

		if (nInsidePointCount == 1 && nOutsidePointCount == 2)
		{
			// Triangle should be clipped. As two points lie outside
			// the plane, the triangle simply becomes a smaller triangle

			// Copy appearance info to new triangle
			out_tri1.col = in_tri.col;

			// The inside point is valid, so keep that...
			out_tri1.p[0] = *inside_points[0];
			out_tri1.t[0] = *inside_tex[0];

			// but the two new points are at the locations where the
			// original sides of the triangle (lines) intersect with the plane
			float t;
			out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
			out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
			out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
			out_tri1.t[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
			out_tri1.t[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
			out_tri1.t[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;
			out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;

			return 1; // Return the newly formed single triangle
		}

		if (nInsidePointCount == 2 && nOutsidePointCount == 1)
		{
			// Triangle should be clipped. As two points lie inside the plane,
			// the clipped triangle becomes a "quad". Fortunately, we can
			// represent a quad with two new triangles

			// Copy appearance info to new triangles
			out_tri1.col = in_tri.col;
			out_tri2.col = in_tri.col;

			// The first triangle consists of the two inside points and a new
			// point determined by the location where one side of the triangle
			// intersects with the plane
			out_tri1.p[0] = *inside_points[0];
			out_tri1.p[1] = *inside_points[1];
			out_tri1.t[0] = *inside_tex[0];
			out_tri1.t[1] = *inside_tex[1];

			float t;
			out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
			out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
			out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
			out_tri1.t[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

			// The second triangle is composed of one of he inside points, a
			// new point determined by the intersection of the other side of the
			// triangle and the plane, and the newly created point above
			out_tri2.p[0] = *inside_points[1];
			out_tri2.t[0] = *inside_tex[1];
			out_tri2.p[1] = out_tri1.p[2];
			out_tri2.t[1] = out_tri1.t[2];
			out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
			out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
			out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;
			out_tri2.t[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;
			return 2; // Return two newly formed triangles which form a quad
		}
	}

	// Input parameter lum must be between 0 and 1 - i.e. [0, 1]
	olc::Pixel GetColour(float lum) {

		int nValue = (int)(std::max(lum, 0.20f) * 255.0f);
		return olc::Pixel(nValue, nValue, nValue);
	}

	float* pDepthBuffer = nullptr;

public:
	bool OnUserCreate() override
	{

		pDepthBuffer = new float[ScreenWidth() * ScreenHeight()];

		// Load object file
		meshCube.LoadFromObjectFile("mountains.obj");

		sprTex1 = new olc::Sprite("rodent.png");

		// Projection Matrix
		matProj = mat4x4::projection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::UP).bHeld)
			vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards

		if (GetKey(olc::Key::DOWN).bHeld)
			vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards


		// Dont use these two in FPS mode, it is confusing :P
		if (GetKey(olc::Key::LEFT).bHeld)
			vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

		if (GetKey(olc::Key::RIGHT).bHeld)
			vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis
		///////


		vec3d vForward = vLookDir * (8.0f * fElapsedTime);

		// Standard FPS Control scheme, but turn instead of strafe
		if (GetKey(olc::Key::W).bHeld)
			vCamera = vCamera + vForward;

		if (GetKey(olc::Key::S).bHeld)
			vCamera = vCamera - vForward;

		if (GetKey(olc::Key::A).bHeld)
			fYaw -= 2.0f * fElapsedTime;

		if (GetKey(olc::Key::D).bHeld)
			fYaw += 2.0f * fElapsedTime;

		fTheta += 1.0f * fElapsedTime;

		// Start position
		mat4x4 matWorld = mat4x4::identity();

		// Create "Point At" Matrix for camera
		vec3d vUp = { 0,1,0 };
		vec3d vTarget = { 0,0,1 };
		mat4x4 matCameraRot = mat4x4::identity();
		matCameraRot.applyRotationY(-fYaw);
		vLookDir = matCameraRot * vTarget;
		vTarget = vCamera + vLookDir;
		mat4x4 matCamera = mat4x4::pointingAt(vCamera, vTarget, vUp);

		// Make view matrix from camera
		mat4x4 matView = matCamera.quickInverse();

		// Store triagles for rastering later
		std::vector<triangle> vecTrianglesToRaster;

		// Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			for (int i = 0; i < 3; i++) {
				triTransformed.p[i] = matWorld * tri.p[i];
				triTransformed.t[i] = tri.t[i];
			}

			// Calculate triangle Normal
			vec3d normal, line1, line2;

			// Get lines either side of triangle
			line1 = triTransformed.p[1] - triTransformed.p[0];
			line2 = triTransformed.p[2] - triTransformed.p[0];

			// Take cross product of lines to get normal to triangle surface
			normal = line1.cross(line2);

			// You normally need to normalise a normal!
			normal.normalize();

			// Get Ray from triangle to camera
			vec3d vCameraRay = triTransformed.p[0] - vCamera;

			// If ray is aligned with normal, then triangle is visible
			if (normal.dot(vCameraRay) < 0.0f)
			{
				// Illumination
				vec3d light_direction = { 0.0f, 1.0f, -1.0f };
				light_direction.normalize();

				// How "aligned" are light direction and triangle surface normal?
				float dp = std::max(0.1f, light_direction.dot(normal));

				// Choose console colours as required (much easier with RGB)
				triTransformed.col = GetColour(dp);

				// Convert World Space --> View Space
				triViewed.col = triTransformed.col;
				for (int i = 0; i < 3; i++) {
					triViewed.p[i] = matView * triTransformed.p[i];
					triViewed.t[i] = triTransformed.t[i];
				}

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles.
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				// We may end up with multiple triangles form the clip, so project as
				// required
				for (int n = 0; n < nClippedTriangles; n++)
				{
					
					triProjected.col = clipped[n].col;
					for (int i = 0; i < 3; i++) {
						// Project triangles from 3D --> 2D
						triProjected.p[i] = matProj * clipped[n].p[i];
						triProjected.t[i] = clipped[n].t[i];

						triProjected.t[i].w = 1.0f;
						triProjected.t[i] /= triProjected.p[i].w;

						// Scale into view, we moved the normalising into cartesian space
						// out of the matrix.vector function from the previous videos, so
						// do this manually
						triProjected.p[i] = triProjected.p[i] / triProjected.p[i].w;

						// X/Y are inverted so put them back
						triProjected.p[i].x *= -1.0f;
						triProjected.p[i].y *= -1.0f;

						// Offset verts into visible normalised space
						const vec3d vOffsetView = { 1,1,0 };
						triProjected.p[i] = triProjected.p[i] + vOffsetView;
						triProjected.p[i].x *= 0.5f * (float)ScreenWidth();
						triProjected.p[i].y *= 0.5f * (float)ScreenHeight();
					}

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}
			}
		}

		// Sort triangles from back to front
		/*sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});*/

		// Clear Screen
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::CYAN);

		// Clear Depth Buffer
		for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
			pDepthBuffer[i] = 0.0f;


		// Loop through all transformed, viewed, projected, and sorted triangles
		for (auto& triToRaster : vecTrianglesToRaster)
		{
			// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to
			//  ensure we only test new triangles generated against planes
			triangle clipped[2];
			std::list<triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clip it against a plane. We only need to test each
					// subsequent plane, against subsequent new triangles
					// as all triangles after a plane clip are guaranteed
					// to lie on the inside of the plane. I like how this
					// comment is almost completely and utterly justified
					switch (p)
					{
						case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f,						0.0f,						0.0f }, { +0.0f, +1.0f, +0.0f }, test, clipped[0], clipped[1]); break;
						case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f,						(float)ScreenHeight() - 1,	0.0f }, { +0.0f, -1.0f, +0.0f }, test, clipped[0], clipped[1]); break;
						case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f,						0.0f,						0.0f }, { +1.0f, +0.0f, +0.0f }, test, clipped[0], clipped[1]); break;
						case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1,	0.0f,						0.0f }, { -1.0f, +0.0f, +0.0f }, test, clipped[0], clipped[1]); break;
					}

					// Clipping may yield a variable number of triangles, so
					// add these new ones to the back of the queue for subsequent
					// clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}

			// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
			for (auto& t : listTriangles)
			{
				TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0],
					t.p[1].x, t.p[1].y, t.t[1],
					t.p[2].x, t.p[2].y, t.t[2], sprTex1);

				//DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, olc::WHITE);
			}
		}

		return true;
	}

	void TexturedTriangle(int x1, int y1, vec2d t1,
						  int x2, int y2, vec2d t2,
						  int x3, int y3, vec2d t3,
						  olc::Sprite* tex)
	{
		if (y2 < y1)
		{
			std::swap(y1, y2);
			std::swap(x1, x2);
			std::swap(t1, t2);
		}

		if (y3 < y1)
		{
			std::swap(y1, y3);
			std::swap(x1, x3);
			std::swap(t1, t3);
		}

		if (y3 < y2)
		{
			std::swap(y2, y3);
			std::swap(x2, x3);
			std::swap(t2, t3);
		}

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;
		vec2d d1 = t2 - t1;

		int dy2 = y3 - y1;
		int dx2 = x3 - x1;
		vec2d d2 = t3 - t1;

		vec2d texPos;

		float dax_step = 0, dbx_step = 0;

		vec2d d1_step = { 0, 0, 0 };
		vec2d d2_step = { 0, 0, 0 };

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		if (dy1) d1_step = d1 / (float)abs(dy1);
		if (dy2) d2_step = d2 /(float)abs(dy2);

		if (dy1)
		{
			for (int i = y1; i <= y2; i++)
			{
				DrawHalfTriangle(x1, y1, x2, y2, i, t1, t2, dax_step, dbx_step, d1_step, d2_step, texPos, tex);
			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		d1 = t3 - t2;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		d1_step.u = 0;
		d1_step.v = 0;

		if (dy1) d1_step = d1 / (float)abs(dy1);

		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				DrawHalfTriangle(x1, y1, x2, y2, i, t1, t2, dax_step, dbx_step, d1_step, d2_step, texPos, tex);
			}
		}
	}

	void DrawHalfTriangle(int x1, int y1, int x2, int y2, int i, vec2d t1, vec2d t2, float dax_step, float dbx_step, vec2d d1_step, vec2d d2_step, vec2d& texPos, olc::Sprite* tex)
	{
		int ax = x2 + (float)(i - y2) * dax_step;
		int bx = x1 + (float)(i - y1) * dbx_step;

		vec2d texS = t2 + d1_step * (float)(i - y2);
		vec2d texE = t1 + d2_step * (float)(i - y1);

		if (ax > bx)
		{
			std::swap(ax, bx);
			std::swap(texS, texE);
		}

		texPos = texS;

		float tstep = 1.0f / ((float)(bx - ax));
		float t = 0.0f;

		for (int j = ax; j < bx; j++)
		{
			texPos = texS * (1.0f - t) + texE * t;

			if (texPos.w > pDepthBuffer[i * ScreenWidth() + j])
			{
				Draw(j, i, tex->Sample(texPos.u / texPos.w, texPos.v / texPos.w));
				pDepthBuffer[i * ScreenWidth() + j] = texPos.w;
			}
			t += tstep;
		}
	}
};

int main()
{
	olcEngine3D demo;
	if (demo.Construct(1280, 720, 1, 1))
		demo.Start();
	return 0;
}