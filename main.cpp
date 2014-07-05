//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : <ZUGOR> <MARTON>
// Neptun : <MA8WE1>
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>     
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>     


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

const float PI = 3.14159265;
int text_id_factory = 0;
float time = 0;
const float g = 0.04;
bool isBallFired = false;

//===================================================
struct Vector
//===================================================
{
	float x, y, z;

	Vector( )
	{
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0)
	{
		x = x0;
		y = y0;
		z = z0;
	}
	Vector operator*(float a)
	{
		return Vector(x * a, y * a, z * a);
	}
	Vector operator+(const Vector& v)
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v)
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v)   	// dot product
	{
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v)   	// cross product
	{
		return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	float Length()
	{
		return sqrt(x * x + y * y + z * z);
	}
	Vector normalize()
	{
		return Vector(x/Length(),y/Length(),z/Length());
	}
};

Vector v_heli;
Vector v_lufi = Vector(0.0,0.0,0.0);

//===================================================
struct Color
//===================================================
{
	float r, g, b;

	Color( )
	{
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0)
	{
		r = r0;
		g = g0;
		b = b0;
	}
	Color operator*(float a)
	{
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c)
	{
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c)
	{
		return Color(r + c.r, g + c.g, b + c.b);
	}
};

//===================================================
class Quaternion
//===================================================
{
public:
	float s;
	Vector d;

	Quaternion() {};

	Quaternion(float _s, Vector _d)
	{
		s = _s;
		d = _d;
	}

	Quaternion operator*(float f)
	{
		return Quaternion(s*f, d*f);
	}

	void Normalize()
	{
		float length = sqrt(s*s + d.x * d.x + d.y*d.y + d.z * d.z);
		(*this) = (*this) * (1/length);
	}

	float Length()
	{
		return sqrt(s*s + d.x * d.x + d.y*d.y + d.z * d.z);
	}

	Quaternion operator%(Quaternion q)
	{
		return Quaternion(s*q.s - d*q.d, q.d * s + d*q.s + d % q.d);
	}

	float GetRotarionAngle()
	{
		float cosa2 = s, sina2 = d.Length();
		float angRad = atan2(sina2, cosa2) * 2;
		return angRad * 180 / PI;
	}
};

float camPos=0.0;
//===================================================
class Camera
//===================================================
{
public:
	Vector eye,lookat,vup,viewport;
	float fov,asp,fp,bp;
	float speed;

	Camera() {};

	Camera(Vector _eye,Vector _lookat, Vector _vup,float _fov,float _asp,float _fp,float _bp,Vector _viewport)
	{
		eye = _eye;
		lookat = _lookat;
		vup = _vup;
		fov  = _fov;
		asp = _asp;
		fp = _fp;
		bp = _bp;
		viewport = _viewport;

		speed = 0;
	}

	void setOpenGL()
	{
		glViewport(viewport.x, viewport.y, viewport.z, viewport.z );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity( );
		gluPerspective(fov, asp, fp, bp);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity( );
		gluLookAt(eye.x, eye.y, eye.z, lookat.x, lookat.y, lookat.z, vup.x, vup.y, vup.z);
		glMatrixMode(GL_PROJECTION);
	}

	void VLufiCalc(Vector helipos, float part)
	{
		Vector dist = helipos - eye;
		float stretch = dist.Length() - 2.2;
		if(stretch < 0) stretch = 0;
		float Fpull = stretch * 10; //10 newton/m rugóállandó
		Vector dir0 = dist.normalize();
		Vector pull = (dir0 * Fpull) * (1.0 / 5.0); //5kg kamera

		Vector lift = Vector(0.0, 0.05, 0.0);

		Vector drag = v_heli * -4.0;

		v_lufi = pull + lift + drag;

	}

	void Animate(float part)
	{
		eye = eye + v_lufi * part;
	}

};

//===================================================
class Material
//===================================================
{
public:
	float shininess;
	float k_diffuse[4];
	float k_specular[4];
	float k_ambient[4];

	Material() {};

	Material(float diffuse[4],float specular[4],float ambient[4], float s0)
	{
		for(int i=0; i<4; i++)
		{
			k_diffuse[i] = diffuse[i];
			k_specular[i] = specular[i];
			k_ambient[i] = ambient[i];
		}
		shininess = s0;
	}

	void setOpenGL()
	{
		glMaterialfv(GL_FRONT , GL_AMBIENT, k_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, k_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, k_specular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}
};

//===================================================
class Light
//===================================================
{
public:
	float L_diffuse[4];
	float L_ambient[4];
	float L_specular[4];
	float pos[4];

	Light() {};

	Light(float diffuse[4], float ambient[4], float specular[4], float _pos[4])
	{
		for(int i=0; i<4; i++)
		{
			L_diffuse[i] = diffuse[i];
			L_ambient[i] = ambient[i];
			L_specular[i] = specular[i];
			pos[i] = _pos[i];
		}
	}

	void setOpenGL()
	{
		glLightfv(GL_LIGHT1, GL_AMBIENT, L_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, L_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, L_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, pos);
		glEnable(GL_LIGHT1);
	}
};

//===================================================
class Texture
//===================================================
{
public:
	unsigned int text_id;

	Texture() {};

	float textureGenerator()
	{
		return (rand() % 21893) / 21893.0;
	}

	void Load()
	{
		float textureArray[64][64][3];

		for(int i=0; i<64; i++)
			for(int j=0; j<64; j++)
			{
				float c = textureGenerator();
				textureArray[i][j][0] = c/1.2;
				textureArray[i][j][1] = c/1.2;
				textureArray[i][j][2] = c/1.3;
			}		

		text_id = (text_id_factory);
		text_id_factory++;

		glGenTextures(1, &text_id);
		glBindTexture(GL_TEXTURE_2D, text_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureArray);
	}
	
	void setOpenGL()
	{
		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, text_id);
	}
};

Texture* terrainTexture = new Texture();

//===================================================
class Primitive
//===================================================
{
public:
	Material material;
	Texture* texture;
	bool textureEnable;

	Primitive() {};

	Primitive(Material _material, Texture* _texture, bool _textureEnable)
	{
		material= _material;
		texture= _texture;
		textureEnable = _textureEnable;
	}

	void tessDrawOpenGL()
	{
		glPushMatrix();
		material.setOpenGL();
		if(textureEnable)
			texture->setOpenGL();
		glMatrixMode(GL_MODELVIEW);
		draw();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	virtual void draw()=0;
};

//===================================================
class TerrainPrimitive: public Primitive
//===================================================
{
public:
	float terr_base[128][128];

	TerrainPrimitive(Material _material, Texture* _texture, bool en):Primitive(_material, _texture, en)
	{
	}

	void draw()
	{
		glPushMatrix();
		Vector myNormal;

		for(int i=0; i<127; i++)
			for(int j=0; j<63; j++)
			{
				myNormal = Vector(0.0, 1.0, 0.0);
				glBegin(GL_POLYGON);
				glNormal3f(myNormal.x, myNormal.y, myNormal.z);
				glTexCoord2f(0.0,0.0);
				glVertex3f((float)i*(10.0/128.0)-5.0,	 -0.5,	  (float)j*(5.0/64.0)-2.5);
				glTexCoord2f(0.0,1.0);
				glVertex3f((float)i*(10.0/128.0)-5.0,	 -0.5,   (float)(j+1)*(5.0/64.0)-2.5);
				glTexCoord2f(1.0,1.0);
				glVertex3f((float)(i+1)*(10.0/128.0)-5.0, -0.5, (float)(j+1)*(5.0/64.0)-2.5);
				glTexCoord2f(1.0,0.0);
				glVertex3f((float)(i+1)*(10.0/128.0)-5.0, -0.5,   (float)j*(5.0/64.0)-2.5);
				glEnd();
			}
		glPopMatrix();
	}
};

//===================================================
class Cylinder: public Primitive
//===================================================
{
public:
	float radius;
	Vector botCenter, topCenter;
	Cylinder(float _radius, Vector _botCenter, Vector _topCenter, Material _material, Texture* _texture, bool en):Primitive(_material, _texture, en)
	{
		radius = _radius;
		botCenter = _botCenter;
		topCenter = _topCenter;
	}

	void draw()
	{
		Vector e = topCenter - botCenter;
		Vector firstNorm = e % topCenter;
		firstNorm = firstNorm.normalize();
		Vector secondNorm = e % firstNorm;
		secondNorm = secondNorm.normalize();

		Vector samplePoints[128];
		for(int i=0; i<127; i++)
		{
			float angle = (2.0*PI)*(i/128.0);
			samplePoints[i].x = (float)(radius*(cosf(angle)*firstNorm.x+sinf(angle)*secondNorm.x));
			samplePoints[i].y = (float)(radius*(cosf(angle)*firstNorm.y+sinf(angle)*secondNorm.y));
			samplePoints[i].z = (float)(radius*(cosf(angle)*firstNorm.z+sinf(angle)*secondNorm.z));
		}
		samplePoints[127].x = samplePoints[0].x;
		samplePoints[127].y = samplePoints[0].y;
		samplePoints[127].z = samplePoints[0].z;
		
		glPushMatrix();
		glTranslatef(botCenter.x, botCenter.y, botCenter.z);

		//also
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(-e.normalize().x, -e.normalize().y, -e.normalize().z);
		glVertex3f(0.0, 0.0, 0.0);
        for(int i = 0; i < 128; i++) {
			glTexCoord2f(1.0, 0.0);
			glVertex3f(samplePoints[i].x, samplePoints[i].y, samplePoints[i].z);
        }
		glEnd();

		//felso
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(e.normalize().x, e.normalize().y, e.normalize().z);
		glVertex3f(e.x, e.y, e.z);
        for(int i = 0; i < 128; i++) {
			glTexCoord2f(0.0, 1.0);
            glVertex3f(samplePoints[i].x + e.x, samplePoints[i].y + e.y, samplePoints[i].z + e.z);
        }
		glEnd();

		//palast
		glBegin(GL_QUAD_STRIP);
        for(int i = 0; i < 128; i++)
		{
			glNormal3f(samplePoints[i].x, samplePoints[i].y, samplePoints[i].z);
			glTexCoord2f(1.0,1.0);
			glVertex3f(samplePoints[i].x + e.x, samplePoints[i].y + e.y, samplePoints[i].z + e.z);
			glTexCoord2f(0.0,0.0);
			glVertex3f(samplePoints[i].x, samplePoints[i].y, samplePoints[i].z);
        }
		glEnd();

		glPopMatrix();
	}
};

//===================================================
class Cone: public Primitive
//===================================================
{
public:
	float radius;
	Vector botCenter, topCenter;
	Cone(float _radius, Vector _botCenter, Vector _topCenter, Material _material, Texture* _texture, bool en):Primitive(_material, _texture, en)
	{
		radius = _radius;
		botCenter = _botCenter;
		topCenter = _topCenter;
	};

	void draw()
	{
		Vector e = topCenter-botCenter;
		Vector firstNorm = e % topCenter;
		firstNorm = firstNorm.normalize();
		Vector secondNorm = e % firstNorm;
		secondNorm=secondNorm.normalize();

		Vector samplePoints[128];
		for(int i=0; i<127; i++)
		{
			float angle = (2.0*PI)*(i/128.0);
			samplePoints[i].x = (float)(radius*(cosf(angle)*firstNorm.x+sinf(angle)*secondNorm.x));
			samplePoints[i].y = (float)(radius*(cosf(angle)*firstNorm.y+sinf(angle)*secondNorm.y));
			samplePoints[i].z = (float)(radius*(cosf(angle)*firstNorm.z+sinf(angle)*secondNorm.z));
		}
		samplePoints[127].x = samplePoints[0].x;
		samplePoints[127].y = samplePoints[0].y;
		samplePoints[127].z = samplePoints[0].z;

		glPushMatrix();
		//also
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(e.normalize().x, e.normalize().y, e.normalize().z);
		glVertex3f(botCenter.x, botCenter.y, botCenter.z);
        for(int i = 0; i < 128; i++) {
			glTexCoord2f(1.0, 0.0);
			glVertex3f(botCenter.x + samplePoints[i].x, botCenter.y + samplePoints[i].y, botCenter.z + samplePoints[i].z);
        }
		glEnd();

		//palast
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(topCenter.x, topCenter.y, topCenter.z);
        for(int i=0; i<128; i++)
		{
			Vector tmp1, tmp2, tmp;
			tmp1.x = samplePoints[i].x-botCenter.x;
			tmp1.y = samplePoints[i].y-botCenter.y;
			tmp1.z = samplePoints[i].z-botCenter.z;
			tmp1 = tmp1.normalize();
			if(i != 0)
			{
				tmp2.x = samplePoints[i].x-samplePoints[i-1].x;
				tmp2.y = samplePoints[i].y-samplePoints[i-1].y;
				tmp2.z = samplePoints[i].z-samplePoints[i-1].z;
			}
			else
			{
				tmp2.x = samplePoints[0].x-samplePoints[127].x;
				tmp2.y = samplePoints[0].y-samplePoints[127].y;
				tmp2.z = samplePoints[0].z-samplePoints[127].z;
			}
			tmp2.normalize();
			tmp = tmp2%tmp1;
			glNormal3f(-tmp.x, -tmp.y, -tmp.z);
			glTexCoord2f(0.0,0.0);
			glVertex3f(botCenter.x + samplePoints[i].x, botCenter.y + samplePoints[i].y, botCenter.z + samplePoints[i].z);
        }
		glEnd();

		glPopMatrix();
	};
};


//===================================================
class Sphere: public Primitive
//===================================================
{
public:
	Vector center;
	float radius;

	Sphere(Vector _center, float _radius, Material _material, Texture* _texture, bool en):Primitive(_material, _texture, en)
	{
		center = _center;
		radius = _radius;
	}

	void draw()
	{
		glPushMatrix();
		glTranslatef(center.x, center.y, center.z);

		for(int i=0; i<40; i++)
		{
			float l1 = PI * (-0.5 + (float)(i-1)/40.0);
			float z0  = sinf(l1);
			float zr0 =  cosf(l1);
			float l2 = PI * (-0.5 + (float)(i)/40.0);
			float z1  = sinf(l2);
			float zr1 =  cosf(l2);

			glBegin(GL_QUAD_STRIP);
			for(int j=0; j <= 40; j++)
			{
				float lng = 2 * PI * (float)(j-1)/40.0;
				float x = cos(lng)/radius;
				float y = sin(lng)/radius;
				glNormal3f(x * zr0, y * zr0, z0/radius);
				glVertex3f((x * zr0), (y * zr0),(z0/radius));
				glNormal3f(x * zr1, y * zr1, z1/radius);
				glVertex3f((x * zr1),(y * zr1),( z1/radius));
			}
        glEnd();
		}

		glPopMatrix();
	}
};


//===================================================
class Ellipsoid: public Primitive
//===================================================
{
public:
	Vector center;
	float radius;
	int cockpit;

	Ellipsoid(Vector _center, float _radius, int _cockpit, Material _material, Texture* _texture, bool en):Primitive(_material, _texture, en)
	{
		center = _center;
		radius = _radius;
		cockpit = _cockpit;
	}

	void draw()
	{
		glPushMatrix();
		glTranslatef(center.x, center.y, center.z);

		for(int i=0; i<40; i++)
		{
			float l1 = PI * (-0.5 + (float)(i-1)/40.0);
			float z0  = sinf(l1);
			float zr0 =  cosf(l1);
			float l2 = PI * (-0.5 + (float)(i)/40.0);
			float z1  = sinf(l2);
			float zr1 =  cosf(l2);

			glBegin(GL_QUAD_STRIP);
			for(int j=0; j <= 40; j++)
			{
				float lng;
				if(cockpit == 1) lng = ((2 * PI)/5.0) * (float)(j-4)/40.0;
				else lng = ((8 * PI)/5.0) * (float)(j-1)/40.0 + ((2 * PI)/5.0);
				float x = (cos(lng)/radius)*2.0;
				float y = sin(lng)/radius;
				glNormal3f(x * zr0, y * zr0, z0/radius);
				glVertex3f((x * zr0), (y * zr0),(z0/radius));
				glNormal3f(x * zr1, y * zr1, z1/radius);
				glVertex3f((x * zr1),(y * zr1),( z1/radius));
			}
        glEnd();
		}

		glPopMatrix();
	}
};

//===================================================
class Cuboid: public Primitive
//===================================================
{
public:
	Vector botCenter, topCenter;
	int orientation;
	Cuboid(Vector _botCenter, Vector _topCenter, int _orientation, Material _material, Texture* _texture, bool en):Primitive(_material, _texture, en)
	{
		orientation = _orientation; //0 - fektetett | 1 - élére döntött
		botCenter = _botCenter;
		topCenter = _topCenter;
	}

	void draw()
	{
		Vector e = topCenter - botCenter;
		Vector firstNorm = e % topCenter;
		firstNorm = firstNorm.normalize();
		Vector secondNorm = e % firstNorm;
		secondNorm = secondNorm.normalize();

		if(botCenter.x > topCenter.x)
		{
			firstNorm = firstNorm * -1.0;
		}

		Vector points[8];
		if(orientation == 0)
		{
			points[0] = Vector(0.02 * (firstNorm.x + secondNorm.x),  0.02 * (firstNorm.y + secondNorm.y),  0.02 * (firstNorm.z + secondNorm.z));
			points[1] = Vector(0.02828 * (firstNorm.x + secondNorm.x),	0.02828 * (firstNorm.y + secondNorm.y), 0.02 * (firstNorm.z + secondNorm.z));
			points[2] = Vector(0.02828 * (firstNorm.x + secondNorm.x),	0.02828 * (firstNorm.y + secondNorm.y), -0.02 * (firstNorm.z + secondNorm.z));
			points[3] = Vector(0.02 * (firstNorm.x + secondNorm.x),	0.02 * (firstNorm.y + secondNorm.y),  -0.02 * (firstNorm.z + secondNorm.z));
		}
		else
		{
			points[0] = Vector(0.02 * (firstNorm.x + secondNorm.x),		0.02 * (firstNorm.y + secondNorm.y),	  0.02 * (firstNorm.z + secondNorm.z));
			points[1] = Vector(0.02828 * (firstNorm.x + secondNorm.x),		0.02 * (firstNorm.y + secondNorm.y),  0.02828 * (firstNorm.z + secondNorm.z));
			points[2] = Vector(0.02828 * (firstNorm.x + secondNorm.x),		-0.02 * (firstNorm.y + secondNorm.y), 0.02828 * (firstNorm.z + secondNorm.z));
			points[3] = Vector(0.02 * (firstNorm.x + secondNorm.x),		-0.02 * (firstNorm.y + secondNorm.y),  0.02 * (firstNorm.z + secondNorm.z));
		}
		points[4] = points[0] + e;
		points[5] = points[1] + e;
		points[6] = points[2] + e;
		points[7] = points[3] + e;
		
		glPushMatrix();
		glTranslatef(botCenter.x, botCenter.y, botCenter.z);

		//also
		glBegin(GL_TRIANGLE_FAN);
			glNormal3f(-e.normalize().x, -e.normalize().y, -e.normalize().z);
			glVertex3f(points[0].x, points[0].y, points[0].z);
			for(int i = 1; i < 4; i++) {
				glTexCoord2f(1.0, 0.0);
				glVertex3f(points[i].x, points[i].y, points[i].z);
			}
		glEnd();

		//felso
		glBegin(GL_TRIANGLE_FAN);
			glNormal3f(e.normalize().x, e.normalize().y, e.normalize().z);
			glVertex3f(points[4].x, points[4].y, points[4].z);
			for(int i = 5; i < 8; i++) {
				glTexCoord2f(0.0, 1.0);
				glVertex3f(points[i].x, points[i].y, points[i].z);
			}
		glEnd();

		//elulso
		glBegin(GL_TRIANGLE_FAN);
			if(orientation == 0) glNormal3f(secondNorm.x, secondNorm.y, secondNorm.z);
			else glNormal3f(firstNorm.x, firstNorm.y, firstNorm.z);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(points[1].x, points[1].y, points[1].z);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(points[5].x, points[5].y, points[5].z);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(points[6].x, points[6].y, points[6].z);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(points[2].x, points[2].y, points[2].z);
		glEnd();
		
		//hatso
		glBegin(GL_TRIANGLE_FAN);
			if(orientation == 0) glNormal3f(-secondNorm.x, -secondNorm.y, -secondNorm.z);
			else glNormal3f(-firstNorm.x, -firstNorm.y, -firstNorm.z);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(points[0].x, points[0].y, points[0].z);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(points[4].x, points[4].y, points[4].z);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(points[7].x, points[7].y, points[7].z);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(points[3].x, points[3].y, points[3].z);
		glEnd();

		//baloldali
		glBegin(GL_TRIANGLE_FAN);
			if(orientation == 0) glNormal3f(-firstNorm.x, -firstNorm.y, -firstNorm.z);
			else glNormal3f(secondNorm.x, secondNorm.y, secondNorm.z);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(points[0].x, points[0].y, points[0].z);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(points[4].x, points[4].y, points[4].z);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(points[5].x, points[5].y, points[5].z);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(points[1].x, points[1].y, points[1].z);
		glEnd();

		//jobboldali
		glBegin(GL_TRIANGLE_FAN);
			if(orientation == 0) glNormal3f(firstNorm.x, firstNorm.y, firstNorm.z);
			else glNormal3f(-secondNorm.x, -secondNorm.y, -secondNorm.z);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(points[2].x, points[2].y, points[2].z);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(points[6].x, points[6].y, points[6].z);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(points[7].x, points[7].y, points[7].z);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(points[3].x, points[3].y, points[3].z);
		glEnd();

		glPopMatrix();
	}
};

//===================================================
class Object
//===================================================
{
public:
	Primitive* primitives[11];
	int n;
	Vector botCenter, topCenter;
	Vector botCenter2, topCenter2;
	Vector botCenter3, topCenter3;

	Object() {};

	virtual void Build()=0;
	virtual void SetTransfOpenGL() {}

	virtual void Draw()=0;
	virtual void Animate(float part)=0;
};

//===================================================
class Terrain: public Object
//===================================================
{
public:
	Terrain(){};

	void Build()
	{
		float kd[4] = {0.855, 0.647, 0.125, 1.0};
		float ks[4] = {0.5, 0.5, 0.5, 0.0};
		float ka[4] = {0.855, 0.647, 0.125, 1.0};
		float sh = 20.0;

		Material material = Material(kd, ks, ka, sh);

		primitives[0] = new TerrainPrimitive(material, terrainTexture, true);

		terrainTexture->Load();
	}

	void Draw()
	{
		glPushMatrix();
			primitives[0]->tessDrawOpenGL();
		glPopMatrix();
	}

	void Animate(float part) {};
};

//===================================================
class House: public Object
//===================================================
{
public:
	House(Vector _center)
	{
		botCenter = _center;
		topCenter = botCenter;
		topCenter.y += 0.12;
		topCenter2 = topCenter;
		topCenter2.y += 0.08;
	}

	void Build()
	{
		float kd[4] = {0.8, 0.1, 0.0, 1.0};
		float ks[4] = {0.8, 0.1, 0.0, 1.0};
		float ka[4] = {0.5, 0.1, 0.0, 1.0};
		float sh = 10.0;
		float kd2[4] = {0.9, 0.8, 0.0, 1.0};
		float ks2[4] = {0.9, 0.8, 0.0, 1.0};
		float ka2[4] = {0.9, 0.2, 0.0, 1.0};

		Material material1 = Material(kd, ks, ka, sh);
		Material material2 = Material(kd2, ks2, ka2, sh);

		n = 2;

		primitives[0] = new Cylinder(0.05, botCenter, topCenter, material2, terrainTexture, false);
		primitives[1]= new Cone(0.05, topCenter, topCenter2, material1, terrainTexture, false);
	}

	void Draw()
	{
		glPushMatrix();
			for(int i=0; i<n; i++)
			{
				primitives[i]->tessDrawOpenGL();
			}
		glPopMatrix();
	}

	void Animate(float part) {};
};

//===================================================
class QArrow: public Object
//===================================================
{
public:
	Vector basecenter;

	QArrow(Vector _basecenter, float _len)
	{
		basecenter = _basecenter;
		botCenter = Vector(0.00001, 0.0, 0.0);
		topCenter = botCenter;
		topCenter.y += (_len-0.1);
		botCenter2 = topCenter;
		topCenter2 = botCenter2;
		topCenter2.y += 0.1;
	}

	void Build()
	{
		float kd[4] = {0.5, 0.0, 0.0, 1.0};
		float ks[4] = {0.7, 0.0, 0.0, 1.0};
		float ka[4] = {0.0, 0.0, 0.0, 1.0};
		float sh = 10.0;

		Material material1 = Material(kd, ks, ka, sh);

		n = 2;

		primitives[0] = new Cylinder(0.01, botCenter, topCenter, material1, terrainTexture, false);
		primitives[1] = new Cone(0.015, botCenter2, topCenter2, material1, terrainTexture, false);
	}

	void Draw()
	{
		glPushMatrix();
			for(int i=0; i<n; i++)
			{
				glTranslatef(basecenter.x, basecenter.y, basecenter.z);
				primitives[i]->tessDrawOpenGL();
			}
		glPopMatrix();
	}

	void Animate(float part) {};
};


//===================================================
class Ball: public Object
//===================================================
{
public:
	Vector v;
	float time;
	float bounces;

	Ball()
	{
		botCenter = Vector(0.0,0.0,0.0);
		time = 1.0;
		bounces = 0.0;
	};

	void Build()
	{
		float kd[4] = {0.0, 0.5, 0.0, 1.0};
		float ks[4] = {0.0, 0.7, 0.0, 1.0};
		float ka[4] = {0.0, 0.0, 0.0, 1.0};
		float sh = 10.0;

		Material material1 = Material(kd, ks, ka, sh);

		n = 1;

		primitives[0] = new Sphere(Vector(0.0, 0.0, 0.0), 30.0, material1, terrainTexture, false);
	}

	void Draw()
	{
		glPushMatrix();
			glTranslatef(botCenter.x, botCenter.y, botCenter.z);
			primitives[0]->tessDrawOpenGL();	
		glPopMatrix();
	}

	void Animate(float part)
	{
		botCenter2 = botCenter2 - Vector(0.0, g, 0.0) * part;

		botCenter.x = botCenter.x + botCenter2.x * part;
		botCenter.z = botCenter.z + botCenter2.z * part;
		botCenter.y = botCenter.y + botCenter2.y * part;

		if(botCenter.y <= -0.48)
		{
				botCenter.y = -0.46;
				botCenter2.y *= -0.9;
		}
	}
};


//===================================================
class MainRotor: public Object
//===================================================
{
public:
	Vector center;
	float rotDegree;

	MainRotor(Vector _center)
	{
		center = _center;
		botCenter = botCenter2 = center;
		topCenter = topCenter2 = center;
		topCenter.x += 0.4;
		botCenter2.x -= 0.4;
		rotDegree = 0.0;
	}

	void Build()
	{
		float kd[4] = {0.4, 0.4, 0.4, 1.0};
		float ks[4] = {0.1, 0.1, 0.1, 1.0};
		float ka[4] = {0.4, 0.4, 0.4, 0.0};
		float sh = 10.0;

		Material material1 = Material(kd, ks, ka, sh);

		n = 2;

		primitives[0] = new Cuboid(botCenter, topCenter, 0, material1, terrainTexture, false);
		primitives[1] = new Cuboid(botCenter2, topCenter2, 0, material1, terrainTexture, false);
	}

	void Draw()
	{
		glPushMatrix();
		glRotatef(rotDegree, 0, 1, 0);
		for(int i=0; i<n; i++)
		{
			primitives[i]->tessDrawOpenGL();	
		}
		glPopMatrix();
	}

	void Animate(float part)
	{
		rotDegree += 72 * part;
		if(rotDegree >= 360) rotDegree = 0;
	}
};

//===================================================
class TailRotor: public Object
//===================================================
{
public:
	Vector center;
	float rotDegree;

	TailRotor(Vector _center)
	{
		center = _center;
		botCenter = Vector(0.01, 0.00001, 0.0);
		topCenter = Vector(0.11, 0.00001, 0.0);
		rotDegree = 0.0;
	}

	void Build()
	{
		float kd[4] = {0.4, 0.4, 0.4, 1.0};
		float ks[4] = {0.1, 0.1, 0.1, 1.0};
		float ka[4] = {0.4, 0.4, 0.4, 0.0};
		float sh = 10.0;

		Material material1 = Material(kd, ks, ka, sh);

		n = 3;

		primitives[0] = new Cuboid(botCenter, topCenter, 1, material1, terrainTexture, false);
		primitives[1] = new Cuboid(botCenter, topCenter, 1, material1, terrainTexture, false);
		primitives[2] = new Cuboid(botCenter, topCenter, 1, material1, terrainTexture, false);
	}

	void Draw()
	{
		glPushMatrix();
			glTranslatef(center.x, center.y, center.z);
			glRotatef(rotDegree, 0, 0, 1);
			primitives[0]->tessDrawOpenGL();
			glRotatef(120.0, 0, 0, 1);
			primitives[1]->tessDrawOpenGL();
			glRotatef(120.0, 0, 0, 1);
			primitives[2]->tessDrawOpenGL();
		glPopMatrix();
	}

	void Animate(float part)
	{
		rotDegree += 72 * part;
		if(rotDegree >= 360) rotDegree = 0;
	}
};

//===================================================
class HBody: public Object
//===================================================
{
public:
	HBody()
	{
		botCenter2 = Vector(0.00001, 0.05, 0.0);
		topCenter2 = Vector(0.00001, 0.15, 0.0);
		botCenter3 = Vector(-0.1, 0.00001, 0.0);
		topCenter3 = Vector(-0.6, 0.00001, 0.0);
		botCenter = Vector(-0.2, -0.13, 0.0);
		topCenter = Vector(0.2, -0.13, 0.0);
	};

	void Build()
	{
		float kd[4] = {0.94, 0.718, 0.42, 1.0};
		float ks[4] = {0.94, 0.718, 0.42, 1.0};
		float ka[4] = {0.0, 0.0, 0.0, 0.0};
		float sh = 10.0;
		float kd2[4] = {0.6, 0.6, 0.7, 0.4};
		float ks2[4] = {0.6, 0.6, 0.7, 0.4};
		float ka2[4] = {0.0, 0.0, 0.0, 0.0};
		float kd3[4] = {0.2, 0.2, 0.2, 1.0};
		float ks3[4] = {0.1, 0.1, 0.1, 1.0};
		float ka3[4] = {0.2, 0.2, 0.2, 0.0};

		Material material1 = Material(kd, ks, ka, sh);
		Material material2 = Material(kd2, ks2, ka2, sh);
		Material material3 = Material(kd3, ks3, ka3, sh);
		n = 11;

		Vector tmp1, tmp2, tmp3, tmp4, leg1b, leg2b, leg3b, leg4b, leg1t, leg2t, leg3t, leg4t;
		tmp1 = tmp2 = botCenter;
		tmp3 = tmp4 = topCenter;
		tmp1.z += 0.1; tmp3.z += 0.1;
		tmp2.z -= 0.1; tmp4.z -= 0.1;

		leg1b = Vector(-0.1, -0.13, -0.1);
		leg1t = Vector(-0.1, -0.07, -0.05);
		leg2b = Vector(0.1, -0.13, -0.1);
		leg2t = Vector(0.1, -0.07, -0.05);
		leg3b = Vector(-0.1, -0.13, 0.1);
		leg3t = Vector(-0.1, -0.07, 0.05);
		leg4b = Vector(0.1, -0.13, 0.1);
		leg4t = Vector(0.1, -0.07, 0.05);
		primitives[0] = new Cylinder(0.01, leg1b, leg1t, material3, terrainTexture, false);
		primitives[1] = new Cylinder(0.01, leg2b, leg2t, material3, terrainTexture, false);
		primitives[2] = new Cylinder(0.01, leg3b, leg3t, material3, terrainTexture, false);
		primitives[3] = new Cylinder(0.01, leg4b, leg4t, material3, terrainTexture, false);

		primitives[4] = new Cylinder(0.01, tmp1, tmp3, material3, terrainTexture, false);
		primitives[5] = new Cylinder(0.01, tmp2, tmp4, material3, terrainTexture, false);

		primitives[6] = new Ellipsoid(Vector(0.0,0.0,0.0), 10.0, 0, material1, terrainTexture, false);
		primitives[7] = new Cylinder(0.02, botCenter2, topCenter2, material1, terrainTexture, false);
		primitives[8] = new Cylinder(0.02, botCenter3, topCenter3, material1, terrainTexture, false);
		primitives[9] = new Ellipsoid(Vector(-0.58, 0.0, 0.0), 40.0, 0, material1, terrainTexture, false);
		primitives[10] = new Ellipsoid(Vector(0.0,0.0,0.0), 10.0, 1, material2, terrainTexture, false);

	}

	void Draw()
	{
		glPushMatrix();
			for(int i=0; i<n-1; i++)
			{	
				primitives[i]->tessDrawOpenGL();
			}
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			primitives[10]->tessDrawOpenGL();
			glDisable(GL_BLEND);
		glPopMatrix();
	}

	void Animate(float part) {};
};

//===================================================
class Helicopter
//===================================================
{
public:
	Vector center;
	Object* mainrotor;
	Object* tailrotor;
	Object* hbody;
	Quaternion quaternion;
	float rotAngle, myScale;
	Vector rotAxis;
	Object* qarrow;

	Helicopter(Vector _center)
	{
		center = _center;
		Vector mainrotorCenter = Vector(0.0, 0.175, 0.0);
		hbody = new HBody();
		Vector tailrotorCenter = hbody->topCenter3;
		mainrotor = new MainRotor(mainrotorCenter);
		tailrotor = new TailRotor(tailrotorCenter);
		quaternion = Quaternion(1, Vector(0, 0, 0));
		qarrow = new QArrow(Vector(0.0,0.0,0.0), 0.7);
		rotAngle = 0;
		rotAxis = Vector(0.0,0.0,0.0);
		myScale = 1;
	}

	void rotate(int rotateAxis)
	{
		Vector myAxis;
		float myAngle;
		if(rotateAxis == 0) { myAxis = Vector(1.0, 0.0, 0.0); myAngle = 20.0; }		  //R
		else if(rotateAxis == 1) { myAxis = Vector(1.0, 0.0, 0.0); myAngle = -20.0; } //E
		else if(rotateAxis == 2) { myAxis = Vector(0.0, 0.0, 1.0); myAngle = 20.0; }  //P
		else if(rotateAxis == 3) { myAxis = Vector(0.0, 0.0, 1.0); myAngle = -20.0; } //O
		else if(rotateAxis == 4) { myAxis = Vector(0.0, 1.0, 0.0); myAngle = 20.0; }  //Y
		else if(rotateAxis == 5) { myAxis = Vector(0.0, 1.0, 0.0); myAngle = -20.0; } //X

		myAngle = myAngle * PI / 180.0;
		Quaternion q(cosf(myAngle/2), myAxis * sinf(myAngle/2));
		quaternion = quaternion % q;
		quaternion.Normalize();

		rotAxis = quaternion.d.normalize() % Vector(0.0, 1.0, 0.0);
		float myCosAngle = quaternion.d.normalize() * Vector(0.0, 1.0, 0.0);
		rotAngle = acosf(myCosAngle)*(180/PI);

		myScale = fabs(quaternion.s)*2;
	}

	void Build()
	{
		qarrow->Build();
		mainrotor->Build();
		tailrotor->Build();
		hbody->Build();
	}
	void Draw()
	{
		glPushMatrix();
			glTranslatef(center.x, center.y, center.z);
			//NYIL
			/*glPushMatrix();
				glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
				glScalef(1.0,myScale,1.0);
				qarrow->Draw();
			glPopMatrix();*/
			glRotatef(quaternion.GetRotarionAngle(), quaternion.d.x, quaternion.d.y, quaternion.d.z);
			mainrotor->Draw();
			tailrotor->Draw();
			hbody->Draw();
		glPopMatrix();
	}

	void Animate(float part)
	{
		mainrotor->Animate(part);
		tailrotor->Animate(part);

		Quaternion q0(0, Vector(1.0, 0.0, 0.0));

		Quaternion q1(quaternion.s, quaternion.d * (-1.0));
		q1 = q1 * (1.0 / (q1.Length() * q1.Length()));

		Quaternion q2 = quaternion % q0 % q1;
		Vector v(q2.d.x, q2.d.y, q2.d.z);

		v_heli = v.normalize();
		v_heli = v_heli * 0.02;
		center = center + (v_heli * part);

	}
};

Object* terrain;
Object* ball;
Object* houses[5];
Light* light;
Camera camera1;
Helicopter* helicopter;
//===================================================
class Scene
//===================================================
{
public:
	Camera cam;
	Light* light;
	int object_cnt;

	Scene() {};

	Scene(Camera _cam, Light* _light)
	{
		cam = _cam;
		light = _light;
	}

	void Build()
	{
		for (int i=0; i<5; i++)
		{
			houses[i]->Build();
		}
		terrain->Build();
		ball->Build();
		helicopter->Build();
	}

	void Render()
	{
		light->setOpenGL();
		cam.lookat = helicopter->center;
		cam.setOpenGL();
		for (int i=0; i<5; i++)
		{
			houses[i]->Draw();
		}
		terrain->Draw();
		if(isBallFired)	ball->Draw();
		helicopter->Draw();
	}

	void Animate(float time0, float time1)
	{
		float dt = 100;
		for(float i_time = time0; i_time < time1; i_time += dt)
		{
			float m;
			if((time1-time0) > dt) m = dt;
			else m = (time1-time0);
			float partToDo = m / dt;
			helicopter->Animate(partToDo);
			if(isBallFired) ball->Animate(partToDo);

			cam.VLufiCalc(helicopter->center, partToDo);
			cam.Animate(partToDo);
		}
	}
};

Scene scene;


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	terrain = new Terrain();
	ball = new Ball();
	helicopter = new Helicopter(Vector(0.0, 0.3, 0.0));
	houses[0] = new House(Vector(0.7, -0.5, 0.3));
	houses[1] = new House(Vector(2.5, -0.5, -1.1));
	houses[2] = new House(Vector(-3.0, -0.5, -1.0));
	houses[3] = new House(Vector(1.0, -0.5, 0.7));
	houses[4] = new House(Vector(-1.5, -0.5, 1.2));
	float pos1[4] = { 1.0, 10.0, 1.0, 0.0};
	float tmp_diff1[4] = {1.0, 1.0, 1.0, 1.0};
	float tmp_amb1[4] = {0.0, 0.0, 0.0, 1.0};
	float tmp_spec1[4] = {1.0, 1.0, 1.0, 1.0};
	light = new Light(tmp_diff1, tmp_amb1, tmp_spec1, pos1);

	camera1 = Camera(Vector(-1.8, 1.8, 0.0), helicopter->center, Vector(0.0,1.0,0.0), 50, 1, 0.1, 200, Vector(0.0,0.0,600.0));

	scene = Scene(camera1, light);
	scene.Build();
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	scene.Render();

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet

	if(key == 'R') { helicopter->rotate(0); glutPostRedisplay(); }
	if(key == 'E') { helicopter->rotate(1); glutPostRedisplay(); }
	if(key == 'P') { helicopter->rotate(2); glutPostRedisplay(); }
	if(key == 'O') { helicopter->rotate(3); glutPostRedisplay(); }
	if(key == 'Y') { helicopter->rotate(4); glutPostRedisplay(); }
	if(key == 'X') { helicopter->rotate(5); glutPostRedisplay(); }

	if(key == ' ')
	{
		if(isBallFired == false)
		{
			isBallFired = true;
			ball->botCenter = helicopter->center;
			ball->botCenter3 = helicopter->center;
			ball->botCenter2 = v_heli * 3.0;
		};
	}
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
     float old_time = time;
	 time = glutGet(GLUT_ELAPSED_TIME);
	 scene.Animate(old_time, time);
	 glutPostRedisplay();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}

