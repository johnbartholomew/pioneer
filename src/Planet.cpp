#include "Planet.h"
#include "Pi.h"
#include "WorldView.h"
#include "GeoSphere.h"
#include "perlin.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/Graphics.h"
#include "graphics/Texture.h"
#include "graphics/VertexArray.h"

#ifdef _MSC_VER
	#include "win32/WinMath.h"
	#define log1pf LogOnePlusX
#endif // _MSC_VER

using namespace Graphics;

static const Graphics::AttributeSet RING_VERTEX_ATTRIBS
	= Graphics::ATTRIB_POSITION
	| Graphics::ATTRIB_UV0;

Planet::Planet(): TerrainBody(), m_ringVertices(RING_VERTEX_ATTRIBS)
{
}

Planet::Planet(SystemBody *sbody): TerrainBody(sbody), m_ringVertices(RING_VERTEX_ATTRIBS)
{
	m_hasDoubleFrame = true;
	if (sbody->HasRings()) {
		m_clipRadius = sbody->GetRadius() * sbody->m_rings.maxRadius.ToDouble();
	} else {
		m_clipRadius = GetBoundingRadius();
	}
}

Planet::~Planet() {}

void Planet::Load(Serializer::Reader &rd, Space *space)
{
	TerrainBody::Load(rd, space);

	const SystemBody *sbody = GetSystemBody();
	assert(sbody);
	if (sbody->HasRings()) {
		m_clipRadius = sbody->GetRadius() * sbody->m_rings.maxRadius.ToDouble();
	} else {
		m_clipRadius = GetBoundingRadius();
	}
}

/*
 * dist = distance from centre
 * returns pressure in earth atmospheres
 * function is slightly different from the isothermal earth-based approximation used in shaders,
 * but it isn't visually noticeable.
 */
void Planet::GetAtmosphericState(double dist, double *outPressure, double *outDensity) const
{
#if 0
	static bool atmosphereTableShown = false;
	if (!atmosphereTableShown) {
		atmosphereTableShown = true;
		for (double h = -1000; h <= 50000; h = h+1000.0) {
			double p = 0.0, d = 0.0;
			GetAtmosphericState(h+this->GetSystemBody()->GetRadius(),&p,&d);
			printf("height(m): %f, pressure(kpa): %f, density: %f\n", h, p*101325.0/1000.0, d);
		}
	}
#endif

	double surfaceDensity;
	const double SPECIFIC_HEAT_AIR_CP=1000.5;// constant pressure specific heat, for the combination of gasses that make up air
	// XXX using earth's molar mass of air...
	const double GAS_MOLAR_MASS = 0.02897;
	const double GAS_CONSTANT = 8.3144621;
	const double PA_2_ATMOS = 1.0 / 101325.0;

	// surface gravity = -G*M/planet radius^2
	const double surfaceGravity_g = -G*this->GetSystemBody()->GetMass()/pow((this->GetSystemBody()->GetRadius()),2); // should be stored in sbody
	// lapse rate http://en.wikipedia.org/wiki/Adiabatic_lapse_rate#Dry_adiabatic_lapse_rate
	// the wet adiabatic rate can be used when cloud layers are incorporated
	// fairly accurate in the troposphere
	const double lapseRate_L = -surfaceGravity_g/SPECIFIC_HEAT_AIR_CP; // negative deg/m

	const double height_h = (dist-GetSystemBody()->GetRadius()); // height in m
	const double surfaceTemperature_T0 = this->GetSystemBody()->averageTemp; //K

	Color c;
	GetSystemBody()->GetAtmosphereFlavor(&c, &surfaceDensity);// kg / m^3
	// convert to moles/m^3
	surfaceDensity/=GAS_MOLAR_MASS;

	const double adiabaticLimit = surfaceTemperature_T0/lapseRate_L; //should be stored

	// This model has no atmosphere beyond the adiabetic limit
	if (height_h >= adiabaticLimit) {*outDensity = 0.0; *outPressure = 0.0; return;}

	//P = density*R*T=(n/V)*R*T
	const double surfaceP_p0 = PA_2_ATMOS*((surfaceDensity)*GAS_CONSTANT*surfaceTemperature_T0); // in atmospheres

	// height below zero should not occur
	if (height_h < 0.0) { *outPressure = surfaceP_p0; *outDensity = surfaceDensity*GAS_MOLAR_MASS; return; }

	//*outPressure = p0*(1-l*h/T0)^(g*M/(R*L);
	*outPressure = surfaceP_p0*pow((1-lapseRate_L*height_h/surfaceTemperature_T0),(-surfaceGravity_g*GAS_MOLAR_MASS/(GAS_CONSTANT*lapseRate_L)));// in ATM since p0 was in ATM
	//                                                                               ^^g used is abs(g)
	// temperature at height
	double temp = surfaceTemperature_T0+lapseRate_L*height_h;

	*outDensity = (*outPressure/(PA_2_ATMOS*GAS_CONSTANT*temp))*GAS_MOLAR_MASS;
}

void Planet::GenerateRings(Graphics::Renderer *renderer)
{
	const SystemBody *sbody = GetSystemBody();

	m_ringVertices.Clear();

	// XXX issue #1439 test code
	const bool TEXTURE_NO_REPEAT = Pi::config->Int("TestRingTextureNoRepeat");

	// generate the ring geometry
	const float inner = sbody->m_rings.minRadius.ToFloat();
	const float outer = sbody->m_rings.maxRadius.ToFloat();
	int segments = 200;
	for (int i = 0; i <= segments; ++i) {
		const float a = (2.0f*float(M_PI)) * (float(i) / float(segments));
		const float ca = cosf(a);
		const float sa = sinf(a);
		// XXX issue #1439 test code
		const float texU = TEXTURE_NO_REPEAT ? float(i & 1) : float(i);
		m_ringVertices.Add(vector3f(inner*sa, 0.0f, inner*ca), vector2f(texU, 0.0f));
		m_ringVertices.Add(vector3f(outer*sa, 0.0f, outer*ca), vector2f(texU, 1.0f));
	}

	// XXX issue #1439 test code
	const int RING_TEXTURE_WIDTH = Clamp(Pi::config->Int("TestRingTextureWidth"), 1, 64);

	// generate the ring texture
	const int RING_TEXTURE_HEIGHT = 256;
	ScopedMalloc<unsigned char> buf(malloc(RING_TEXTURE_WIDTH*RING_TEXTURE_HEIGHT*4));

	// XXX issue #1439 test code
	for (int j = 0; j < RING_TEXTURE_HEIGHT; ++j)
	{
		unsigned char* rgba = buf.Get() + 4*j*RING_TEXTURE_WIDTH;
		for (int i = 0; i < RING_TEXTURE_WIDTH; ++i, rgba += 4)
		{
			rgba[0] = j;
			rgba[1] = 0;
			rgba[2] = Clamp((i*256) / RING_TEXTURE_WIDTH, 0, 255);
			rgba[3] = 255;
		}
	}

#if 0
	const float ringScale = (outer-inner)*sbody->GetRadius() / 1.5e7f;

	MTRand rng(GetSystemBody()->seed+4609837);
	Color4f baseCol = sbody->m_rings.baseColor.ToColor4f();
	double noiseOffset = 2048.0 * rng.Double();
	for (int i = 0; i < RING_TEXTURE_LENGTH; ++i) {
		const float alpha = (float(i) / float(RING_TEXTURE_LENGTH)) * ringScale;
		const float n = 0.25 +
			0.60 * noise( 5.0 * alpha, noiseOffset, 0.0) +
			0.15 * noise(10.0 * alpha, noiseOffset, 0.0);

		const float LOG_SCALE = 1.0f/sqrtf(sqrtf(log1pf(1.0f)));
		const float v = LOG_SCALE*sqrtf(sqrtf(log1pf(n)));

		unsigned char *rgba = buf.Get() + i*4;
		rgba[0] = (v*baseCol.r)*255.0f;
		rgba[1] = (v*baseCol.g)*255.0f;
		rgba[2] = (v*baseCol.b)*255.0f;
		rgba[3] = (((v*0.25f)+0.75f)*baseCol.a)*255.0f;
	}

	// first and last pixel are forced to zero, to give a slightly smoother ring edge
	{
		unsigned char *rgba = buf.Get();
		rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;
		rgba = buf.Get() + (RING_TEXTURE_LENGTH-1)*4;
		rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;
	}
#endif

	// XXX issue #1439 test code
	const bool generate_mipmaps = Pi::config->Int("TestRingTextureUseMipmaps");
	const Graphics::TextureSampleMode TEXTURE_WRAP_MODE =
		TEXTURE_NO_REPEAT ? Graphics::LINEAR_CLAMP : Graphics::LINEAR_REPEAT;

	const vector2f texSize(RING_TEXTURE_WIDTH, RING_TEXTURE_HEIGHT);
	const Graphics::TextureDescriptor texDesc(
			Graphics::TEXTURE_RGBA, texSize, TEXTURE_WRAP_MODE, generate_mipmaps);

	m_ringTexture.Reset(renderer->CreateTexture(texDesc));
	m_ringTexture->Update(
			static_cast<void*>(buf.Get()), texSize,
			Graphics::IMAGE_RGBA, Graphics::IMAGE_UNSIGNED_BYTE);
}

void Planet::DrawGasGiantRings(Renderer *renderer, const Camera *camera)
{
	renderer->SetBlendMode(BLEND_ALPHA_PREMULT);
	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT );
	renderer->SetDepthTest(true);
	glEnable(GL_NORMALIZE);

	if (!m_ringTexture) {
		GenerateRings(renderer);
	}

	Material mat;
	mat.unlit = true;
	mat.twoSided = true;
	mat.texture0 = m_ringTexture.Get();
	// XXX should get number of lights through camera when object viewer draw doesn't pass a null pointer
	mat.shader = Graphics::planetRingsShader[Graphics::State::GetNumLights()-1];

	const SystemBody *sbody = GetSystemBody();
	assert(sbody->HasRings());

	renderer->DrawTriangles(&m_ringVertices, &mat, TRIANGLE_STRIP);

	glPopAttrib();
	renderer->SetBlendMode(BLEND_SOLID);
}

void Planet::DrawAtmosphere(Renderer *renderer, const vector3d &camPos)
{
	//this is the non-shadered atmosphere rendering
	Color col;
	double density;
	GetSystemBody()->GetAtmosphereFlavor(&col, &density);

	const double rad1 = 0.999;
	const double rad2 = 1.05;

	glPushMatrix();

	//XXX pass the transform
	matrix4x4d curTrans;
	glGetDoublev(GL_MODELVIEW_MATRIX, &curTrans[0]);

	// face the camera dammit
	vector3d zaxis = (-camPos).Normalized();
	vector3d xaxis = vector3d(0,1,0).Cross(zaxis).Normalized();
	vector3d yaxis = zaxis.Cross(xaxis);
	matrix4x4d rot = matrix4x4d::MakeInvRotMatrix(xaxis, yaxis, zaxis);
	const matrix4x4d trans = curTrans * rot;

	matrix4x4d invViewRot = trans;
	invViewRot.ClearToRotOnly();
	invViewRot = invViewRot.InverseOf();

	//XXX this is always 1
	const int numLights = Pi::worldView->GetNumLights();
	assert(numLights < 4);
	vector3d lightDir[4];
	float lightCol[4][4];
	// only
	for (int i=0; i<numLights; i++) {
		float temp[4];
		glGetLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lightCol[i]);
		glGetLightfv(GL_LIGHT0 + i, GL_POSITION, temp);
		lightDir[i] = (invViewRot * vector3d(temp[0], temp[1], temp[2])).Normalized();
	}

	const double angStep = M_PI/32;
	// find angle player -> centre -> tangent point
	// tangent is from player to surface of sphere
	float tanAng = float(acos(rad1 / camPos.Length()));

	// then we can put the fucking atmosphere on the horizon
	vector3d r1(0.0, 0.0, rad1);
	vector3d r2(0.0, 0.0, rad2);
	rot = matrix4x4d::RotateYMatrix(tanAng);
	r1 = rot * r1;
	r2 = rot * r2;

	rot = matrix4x4d::RotateZMatrix(angStep);

	VertexArray vts(ATTRIB_POSITION | ATTRIB_DIFFUSE | ATTRIB_NORMAL);
	for (float ang=0; ang<2*M_PI; ang+=float(angStep)) {
		const vector3d norm = r1.Normalized();
		const vector3f n = vector3f(norm.x, norm.y, norm.z);
		float _col[4] = { 0,0,0,0 };
		for (int lnum=0; lnum<numLights; lnum++) {
			const float dot = norm.x*lightDir[lnum].x + norm.y*lightDir[lnum].y + norm.z*lightDir[lnum].z;
			_col[0] += dot*lightCol[lnum][0];
			_col[1] += dot*lightCol[lnum][1];
			_col[2] += dot*lightCol[lnum][2];
		}
		for (int i=0; i<3; i++) _col[i] = _col[i] * col[i];
		_col[3] = col[3];
		vts.Add(vector3f(r1.x, r1.y, r1.z), Color(_col[0], _col[1], _col[2], _col[3]), n);
		vts.Add(vector3f(r2.x, r2.y, r2.z), Color(0.f), n);
		r1 = rot * r1;
		r2 = rot * r2;
	}

	Material mat;
	mat.unlit = true;
	mat.twoSided = true;
	mat.vertexColors = true;

	renderer->SetTransform(trans);
	renderer->SetBlendMode(BLEND_ALPHA_ONE);
	renderer->DrawTriangles(&vts, &mat, TRIANGLE_STRIP);
	renderer->SetBlendMode(BLEND_SOLID);

	glPopMatrix();
}

void Planet::SubRender(Renderer *r, const Camera *camera, const vector3d &camPos)
{
	if (GetSystemBody()->HasRings()) { DrawGasGiantRings(r, camera); }
	if (!AreShadersEnabled()) DrawAtmosphere(r, camPos);
}
