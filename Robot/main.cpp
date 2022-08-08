#include "main.h"
#include "TexBox.h"
#include "TextureApp.h"


bool flag = true;
bool flag2 = true;
TexBox*m_Ori;
GLuint	m_Buffer;
GLuint  m_VAO;
GLuint  m_TextureIndex;

GLuint texture;
GLuint texture2;
float windowx;
float windowy;
float positionZ = 0;

int frag = 0;

float ftime = 0;

int windowWidth = 800, windowHeight = 600;

#pragma region testfunction

static float* LoadFloatData(const char* szFile, int* count)
{
	GLint lineCount = 0;
	FILE* fp;
	float* data = 0;

	// 開啟檔案
	fp = fopen(szFile, "r");

	if (fp != NULL)
	{
		char szFloat[1024];

		// 計算檔案的列數
		while (fgets(szFloat, sizeof(szFloat), fp) != NULL)
			lineCount++;

		// 回到檔案的開頭
		rewind(fp);

		// 為檔案中的資料配置 memory space
		data = (float*)malloc((lineCount) * sizeof(float));

		// 讀取檔案中的資料
		if (data != NULL)
		{
			int index = 0;
			while (fgets(szFloat, sizeof(szFloat), fp) != NULL)
			{
				data[index] = (float)atof(szFloat);
				index++;
			}

			// 將 count[0] 設為檔案的列數
			count[0] = index;
		}


		fclose(fp);
	}
	else
		return 0;

	return data;
}

bool LoadBMPTexture(const char* szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	GLint  iWidth, iHeight;

	pBits = gltReadBMPBits(szFileName, &iWidth, &iHeight);
	if (pBits == NULL)
		return false;

	// Set Wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pBits);

	// Do I need to generate mipmaps?
	if (minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST ||
		minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_NEAREST_MIPMAP_NEAREST)
		glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}


#pragma endregion



void sizedata(GLfloat data[],float x, float y, float w, float h, bool mirror /*= true*/)
{
	if (mirror)
	{
		const GLfloat ndata[] =
		{
			x + w	, y,
			x	, y,
			x	, y + h,
			x + w	, y + h,

			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};
		for (int i = 0; i < 16; i++)
		{
			data[i] = ndata[i];
		}
	}
	else 
	{
		const GLfloat ndata[] =
		{
			x + w	, y,
			x	, y,
			x	, y + h,
			x + w	, y + h,

			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
		};
		for (int i = 0; i < 16; i++)
		{
			data[i] = ndata[i];
		}
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(4,3);//以OpenGL version4.3版本為基準
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);//是否向下相容,GLUT_FORWARD_COMPATIBLE不支援(?
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//multisample for golygons smooth
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL 4.3 - Robot");

	glewExperimental = GL_TRUE; //置於glewInit()之前
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);

	//glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(Keyboard);

	int ActionMenu,ModeMenu,ShaderMenu;
	ActionMenu = glutCreateMenu(ActionMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("idle",0);
	glutAddMenuEntry("walk",1);
	glutAddMenuEntry("fly", 2);
	glutAddMenuEntry("aim", 3);

	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ShaderMenu = glutCreateMenu(ShaderMenuEvents);//建立右鍵菜單
	glutAddMenuEntry("Orginal", 0);
	glutAddMenuEntry("NoLight", -1);
	glutAddMenuEntry("Gray", 1);
	glutAddMenuEntry("Gloomy", 2);
	glutAddMenuEntry("Black Circle", 3);
	glutAddMenuEntry("Negative", 4);
	glutAddMenuEntry("Cartoon", 5);
	glutAddMenuEntry("Water Color", 6);
	glutAddMenuEntry("Martian", 7);
	glutAddMenuEntry("Sephia", 8);
	glutAddMenuEntry("Cell Merge", 9);
	glutAddMenuEntry("Floyd Steinberg", 10);
	glutAddMenuEntry("Noise", 11);
	glutAddMenuEntry("Voronoi", 12);
	glutAddMenuEntry("Rotate Particle", 13);
	glutAddMenuEntry("Snow", 14);
	glutAddMenuEntry("Fire", 15);

	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯



	glutCreateMenu(menuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddSubMenu("action",ActionMenu);
	glutAddSubMenu("shader", ShaderMenu);


	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯



	glutMouseFunc(Mouse);
	glutTimerFunc (10, idle, 0); 
	glutMainLoop();
	return 0;
}

void ChangeSize(int w,int h)
{
	windowWidth = w;
	windowHeight = h;
	if(h == 0) h = 1;
	glViewport(0,0,w,h);
	Projection = perspective(80.0f,(float)w/h,0.1f,100.0f);
}

void Mouse(int button,int state,int x,int y)
{
	if(button == 2) isFrame = false;
}

int ActionOrder = 0;
float circle = 0;

void ActionFLY()
{	
	float pY = 3;
	float circleTimes = 25;
	switch (ActionOrder)
	{
	case 0:
		if (angles2[4] < 15)
			angles2[4] += 0.5;

		if (angles2[5] > -15)
			angles2[5] -= 0.5;

		if (angles[12] < 30)
			angles[12]++;

		if (angles[9] > -30)
			angles[9]--;

		if (angles2[6] > -15)
		{
			angles2[6] -= 0.5;
		}
		else
		{
			ActionOrder = 1;
		}
		break;
	case 1:
		if (position < pY)
			position += 0.1;
		else
			ActionOrder = 2;
		break;
	case 2:
		circle += 0.1;
		position = sin(circle) * circleTimes+ pY;
		positionZ = cos(circle) * circleTimes - circleTimes;
		angles[0] = -circle * 180 / 3.14159;
		
		if(circle>3.14159*2)
			ActionOrder = 3;
		break;
	case 3:
		if (position > 0)
			position -= 0.1;
		else
			ActionOrder = 4;
		break;
	case 4:
		angles[0] = 0;

		if (angles2[4] > 0)
			angles2[4] -= 0.5;

		if (angles2[5] < 0)
			angles2[5] += 0.5;

		if (angles[12] > 0)
			angles[12]--;

		if (angles[9] < 0)
			angles[9]++;

		if (angles2[6] < 0)
		{
			angles2[6] += 0.5;
		}
		break;
	}

}

void ActionIDLE()
{
	position = 0;
	positionZ = 0;
	for (int i = 0; i < PARTSNUM; i++)
		angles[i] = 0;
	for (int i = 0; i < PARTSNUM; i++)
		angles2[i] = 0;
}

void ActionAIM()
{
	switch (ActionOrder)
	{
	case 0:
		if (angles[4] > -60)
			angles[4]-=1.5;

		if (angles[11] > -45)
			angles[11] --;
		else
			ActionOrder = 1;

		if (angles2[12] < 90)
			angles2[12] +=2;

		if (angles[12] < 45)
			angles[12] ++;

		if (angles2[0] < 60)
			angles2[0]+=1.5;


		//
		if (angles[3] < 15)
		{
			angles[3]+=0.5;
		}
		if (angles[2] > -15)
		{
			angles[2]-=0.5;
		}
		break;
	case 1:

		if (angles[4] < 0)
			angles[4]++;
		else
			ActionOrder = 1;

		if (angles[11] < 0)
			angles[11] ++;

		if (angles2[12] > 0)
			angles2[12] -= 2;

		if (angles[12] > 0)
			angles[12] --;

		//

		if (angles[5] < 60)
			angles[5]++;
		else
			ActionOrder = 2;

		if (angles[8] < 45)
			angles[8] ++;

		if (angles2[9] < 90)
			angles2[9] += 2;

		if (angles[9] > -45)
			angles[9] --;


		if (angles2[0] > -60)
			angles2[0] -= 2.5;

		//
		if (angles[3] > -15)
		{
			angles[3] -= 1;
		}
		if (angles[2] < 15)
		{
			angles[2] += 1;
		}
		break;
	case 2:
		if (angles[4] > -60)
			angles[4]--;

		if (angles[11] > -45)
			angles[11] --;

		if (angles2[12] < 90)
			angles2[12] += 2;

		if (angles[12] < 30)
			angles[12] ++;
		
		//
		if (angles[5] > 0)
			angles[5]--;

		if (angles[8] > 0)
			angles[8] --;

		if (angles2[9] > 0)
			angles2[9] -= 2;

		if (angles[9] < 0)
			angles[9]++;

		if (angles2[0] < 0)
			angles2[0] += 1.5;

		//
		if (angles[2] > 0)
		{
			angles[2] -= 1;
		}
		if (angles[3] < 0)
		{
			angles[3] += 1;
		}
		break;
	}
}

void idle(int dummy)
{
	isFrame = true;
	int out = 0;
	if (action == WALK)
	{
		updateObj();
	}
	else if (action == IDLE)
	{
		ActionIDLE();
		position = 0;
	}
	else if (action == FLY)
	{
		ActionFLY();
	}
	else if (action==AIM)
	{
		ActionAIM();
	}
	glutPostRedisplay();
	ftime += 0.05;
	glutTimerFunc(10, idle, out);
}

void resetObj(int f)
{
	for(int i = 0 ; i < PARTSNUM;i++)
	{
		angles[i] = 0.0f;
	}
	flag = true;
	flag2 = true;
}

void updateObj()
{
	if (angles[2] == 0)
		flag2 = true;

	if (angles[2] > 15)
	{
		flag = false;
		flag2 = false;
	}
	if (angles[2] < -15)
	{
		flag = true;
		flag2 = false;
	}

	if (flag)
	{
		angles[2] += 0.5;
		angles[3] -= 0.5;
		angles[4] -= 0.5;
		angles[5] -= 0.5;
	}
	else
	{
		angles[2] -= 0.5;
		angles[3] += 0.5;
		angles[4] += 0.5;
		angles[5] += 0.5;
	}
	if (flag2)
		position += 0.01;
	else
		position -= 0.01;


}


 GLuint M_KaID;
 GLuint M_KdID;
 GLuint M_KsID;


void init()
{
	windowx = windowWidth;
	windowy = windowHeight;

	isFrame = false;
	pNo = 0;
	for(int i = 0;i<PARTSNUM;i++)//初始化角度陣列
		angles[i] = 0.0;
	
	//VAO
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "DSPhong_Material.vp" },//vertex shader
		{ GL_FRAGMENT_SHADER, "DSPhong_Material.fp" },//fragment shader
		{ GL_NONE, NULL }};
	program = LoadShaders(shaders);//讀取shader

	glUseProgram(program);//uniform參數數值前必須先use shader

	MatricesIdx = glGetUniformBlockIndex(program,"MatVP");
	ModelID =  glGetUniformLocation(program,"Model");
    M_KaID = glGetUniformLocation(program,"Material.Ka");
	M_KdID = glGetUniformLocation(program,"Material.Kd");
	M_KsID = glGetUniformLocation(program,"Material.Ks");


	modeID = glGetUniformLocation(program, "mode");
	timeID = glGetUniformLocation(program, "ftime");
	windowsizexID = glGetUniformLocation(program, "windowsizex");
	windowsizeyID = glGetUniformLocation(program, "windowsizey");


	texture = TextureApp::GenTexture("background.jpg");
	texture2 = TextureApp::GenTexture("backgroundtoon.jpg");
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	//or
	M_KdID = M_KaID+1;
	M_KsID = M_KaID+2;

	Projection = glm::perspective(80.0f,4.0f/3.0f,0.1f,100.0f);
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	
	// Camera matrix
	View       = glm::lookAt(
		glm::vec3(0,10,25) , // Camera is at (0,10,25), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,1,0 to look upside-down)
		);

	Obj2Buffer();






	//UBO
	glGenBuffers(1,&UBO);
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferData(GL_UNIFORM_BUFFER,sizeof(mat4)*2,NULL,GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(program, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);  
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER,0,UBO,0,UBOsize);
	glUniformBlockBinding(program, MatricesIdx,0);


	glClearColor(1.0,1.0,1.0,1);//black screen
}


#define DOR(angle) (angle*3.1415/180);
void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	

	glBindVertexArray(VAO);
	glUseProgram(program);//uniform參數數值前必須先use shader

	

	float eyey = DOR(eyeAngley);
	View       = lookAt(
		               vec3(eyedistance*sin(eyey),0,eyedistance*cos(eyey)) , // Camera is at (0,0,20), in World Space
		               vec3(0,0,0), // and looks at the origin
		               vec3(0,0.1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		                );
	updateModels();
	//update data to UBO for MVP
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(mat4),&View);
	glBufferSubData(GL_UNIFORM_BUFFER,sizeof(mat4),sizeof(mat4),&Projection);
	glBindBuffer(GL_UNIFORM_BUFFER,0);


	windowx = windowWidth;
	windowy = windowHeight;



	glUniform1i(modeID, frag);
	glUniform1f(windowsizexID, windowx);
	glUniform1f(windowsizeyID, windowy);
	glUniform1f(timeID, ftime);


	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glUniform1i(glGetUniformLocation(program, "tex2"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);




	GLuint offset[3] = {0,0,0};//offset for vertices , uvs , normals
	for(int i = 0;i < PARTSNUM;i++)
	{


		glUniformMatrix4fv(ModelID,1,GL_FALSE,&Models[i][0][0]);
		if (i == 7)
			glUniform1i(glGetUniformLocation(program, "uset"), true);
		else
			glUniform1i(glGetUniformLocation(program, "uset"), false);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,				//location
							  3,				//vec3
							  GL_FLOAT,			//type
							  GL_FALSE,			//not normalized
							  0,				//strip
							  (void*)offset[0]);//buffer offset
		//(location,vec3,type,固定點,連續點的偏移量,buffer point)
		offset[0] +=  vertices_size[i]*sizeof(vec3);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);//location 1 :vec2 UV
		glBindBuffer(GL_ARRAY_BUFFER, uVBO);
		glVertexAttribPointer(1, 
							  2, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[1]);
		//(location,vec2,type,固定點,連續點的偏移量,point)
		offset[1] +=  uvs_size[i]*sizeof(vec2);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);//location 2 :vec3 Normal
		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glVertexAttribPointer(2,
							  3, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[2]);
		//(location,vec3,type,固定點,連續點的偏移量,point)
		offset[2] +=  normals_size[i]*sizeof(vec3);


		int vertexIDoffset = 0;//glVertexID's offset 
		string mtlname;//material name
		vec3 Ks = vec3(1,1,1);//because .mtl excluding specular , so give it here.
		for(int j = 0;j <mtls[i].size() ;j++)
		{//
			mtlname = mtls[i][j];	
			//find the material diffuse color in map:KDs by material name.
			glUniform3fv(M_KdID,1,&KDs[mtlname][0]);
			glUniform3fv(M_KsID,1,&Ks[0]);
			//          (primitive   , glVertexID base , vertex count    )
			glDrawArrays(GL_TRIANGLES, vertexIDoffset  , faces[i][j+1]*3);
			//we draw triangles by giving the glVertexID base and vertex count is face count*3
			vertexIDoffset += faces[i][j+1]*3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	



	}//end for loop for updating and drawing model



	glFlush();//強制執行上次的OpenGL commands
	glutSwapBuffers();//調換前台和後台buffer ,當後臺buffer畫完和前台buffer交換使我們看見它
}


void Obj2Buffer()
{
	std::vector<vec3> Kds;
	std::vector<vec3> Kas;
	std::vector<vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::string texturename;


	loadMTL("Obj/IronMan.mtl", Kds, Kas, Kss, Materials, texturename);

	printf("%d\n", texture);
	for (int i = 0; i<Materials.size(); i++)
	{
		string mtlname = Materials[i];
		//  name            vec3
		KDs[mtlname] = Kds[i];
	}

	load2Buffer("Obj/waist.obj", 0);
	load2Buffer("Obj/body.obj", 1);
	load2Buffer("Obj/leftthigh.obj", 2);
	load2Buffer("Obj/rightthigh.obj", 3);

	load2Buffer("Obj/leftshoulder.obj", 4);

	load2Buffer("Obj/rightshoulder.obj", 5);
	load2Buffer("Obj/head.obj", 6);
	load2Buffer("Obj/BackgroundT.obj", 7);


	load2Buffer("Obj/rightelbow.obj", 8);
	load2Buffer("Obj/righthand.obj", 9);

	load2Buffer("Obj/rightfoot.obj", 10);


	load2Buffer("Obj/leftelbow.obj", 11);
	load2Buffer("Obj/lefthand.obj", 12);

	load2Buffer("Obj/leftfoot.obj", 13);


	GLuint totalSize[3] = {0,0,0};
	GLuint offset[3] = {0,0,0};
	for(int i = 0;i < PARTSNUM ;i++)
	{
		totalSize[0] += vertices_size[i]*sizeof(vec3);
		totalSize[1] += uvs_size[i] * sizeof(vec2);
		totalSize[2] += normals_size[i] * sizeof(vec3);
	}
	//generate vbo
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&uVBO);
	glGenBuffers(1,&nVBO);
	//bind vbo ,第一次bind也同等於 create vbo 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[0],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[1],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[2],NULL,GL_STATIC_DRAW);
	
	
	for(int i = 0;i < PARTSNUM ;i++)
	{
		glBindBuffer(GL_COPY_WRITE_BUFFER,VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[0],vertices_size[i]*sizeof(vec3));
		offset[0] += vertices_size[i]*sizeof(vec3);
		glInvalidateBufferData(VBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[1],uvs_size[i]*sizeof(vec2));
		offset[1] += uvs_size[i]*sizeof(vec2);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[2],normals_size[i]*sizeof(vec3));
		offset[2] += normals_size[i] * sizeof(vec3);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);
	}
	glBindBuffer(GL_COPY_WRITE_BUFFER,0);


}


int asd;
float rrr = 0;
void updateModels()
{
	mat4 Rotatation[PARTSNUM];
	mat4 Translation[PARTSNUM];
	for (int i = 0; i < PARTSNUM; i++) 
	{
		Models[i] = mat4(1.0f);
		Rotatation[i] = mat4(1.0f);
		Translation[i] = mat4(1.0f);
	}

	float r, pitch, yaw, roll;
	float alpha, beta, gamma;

	float eyey = DOR(eyeAngley);
	Rotatation[7] = rotate(eyeAngley, 0, 1, 0);//背景
	Translation[7] = translate(-50*sin(eyey), 0, -50*cos(eyey));
	Models[7] = Translation[7] * Rotatation[7] * scale(5, 5, 1)*scale(windowx / 800, windowy / 600, 1);

	//asd++;

	Rotatation[0] = rotate(rrr, 1, 0, 0)*rotate(angles[0], 1, 0, 0)*rotate(angles2[0], 0, 1, 0);//腰
	Translation[0] = translate(0, position+1, positionZ);
	Models[0] = Translation[0] * Rotatation[0] * scale(0.1, 0.1, 0.1);



	Rotatation[1] = rotate(0, 0, 1, 0);//身體
	Translation[1] = translate(2, 45, 0);
	Models[1] = Models[0] * Translation[1] * Rotatation[1];

	Rotatation[2] = rotate(angles[2], 1, 0, 0)*rotate(angles2[2], 0, 0, 1);//左腿
	Translation[2] = translate(17, 0, 0);
	Models[2] = Models[0] * Translation[2] * Rotatation[2];



	Rotatation[13] = rotate(angles[13], 1, 0, 0);//左腳
	Translation[13] = translate(0,-50, 5);
	Models[13] = Models[2] * Translation[13] * Rotatation[13];



	Rotatation[3] = rotate(0, 1, 0, 0);//右腿
	Translation[3] = translate(-10, 0, 3);
	Models[3] = Models[0] * Translation[3] * Rotatation[3] * rotate(angles[3], 1, 0, 0);

	Rotatation[10] = rotate(0, 1, 0, 0);//右腳
	Translation[10] = translate(0, -50, 5);
	Models[10] = Models[3] * Translation[10] * Rotatation[10];




	//angles[4] = 90;

	Rotatation[4] = rotate(-90, 0, 0, 1) * rotate(angles[4]/*walk aim*/, 0, 1, 0)*rotate(angles2[4]/*fly*/, 0, 0, 1);//左肩
	Translation[4] = translate(32, 25, 0);
	Models[4] = Models[1] * Translation[4] * Rotatation[4];


	Rotatation[11] = rotate(angles[11], 0, 1, 0);//左肘
	Translation[11] = translate(35, 0, 0);
	Models[11] = Models[4] * Translation[11] * Rotatation[11];



	Rotatation[12] = rotate(angles2[12], 1, 0, 0)*rotate(angles[12], 0, 0, 1);//左手
	Translation[12] = translate(38, 0, 0);
	Models[12] = Models[11] * Translation[12] * Rotatation[12];




	Rotatation[5] = rotate(90, 0, 0, 1)*rotate(angles[5], 0, 1, 0)*rotate(angles2[5]/*fly*/, 0, 0, 1);//右肩
	Translation[5] = translate(-35, 20, 0);
	Models[5] = Models[1] * Translation[5] * Rotatation[5];


	asd = 0;
	Rotatation[8] = rotate(angles[8], 0, 1, 0);//右肘
	Translation[8] = translate(-35, 0, 0);
	Models[8] = Models[5] * Translation[8] * Rotatation[8];

	Rotatation[9] = rotate(angles2[9], 1, 0, 0)*rotate(angles[9], 0, 0, 1);//右手
	Translation[9] = translate(-38, 0, 0);
	Models[9] = Models[8] * Translation[9] * Rotatation[9];






	Rotatation[6] = rotate(0, 0, 1, 0)* rotate(angles[6], 1, 0, 0)* rotate(angles2[6], 1, 0, 0);//頭
	Translation[6] = translate(0, 45, 0);
	Models[6] = Models[1] * Translation[6] * Rotatation[6];
}

void load2Buffer(char* obj,int i)
{
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals; // Won't be used at the moment.
	std::vector<unsigned int> materialIndices;

	bool res = loadOBJ(obj, vertices, uvs, normals,faces[i],mtls[i]);
	if(!res) printf("load failed\n");

	//glUseProgram(program);

	glGenBuffers(1,&VBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec3),&vertices[0],GL_STATIC_DRAW);
	vertices_size[i] = vertices.size();

	glGenBuffers(1,&uVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	uvs_size[i] = uvs.size();

	glGenBuffers(1,&nVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	normals_size[i] = normals.size();
}

mat4 translate(float x,float y,float z)
{
	vec4 t = vec4(x,y,z,1);//w = 1 ,則x,y,z=0時也能translate
	vec4 c1 = vec4(1,0,0,0);
	vec4 c2 = vec4(0,1,0,0);
	vec4 c3 = vec4(0,0,1,0);
	mat4 M = mat4(c1,c2,c3,t);
	return M;
}

mat4 scale(float x,float y,float z)
{
	vec4 c1 = vec4(x,0,0,0);
	vec4 c2 = vec4(0,y,0,0);
	vec4 c3 = vec4(0,0,z,0);
	vec4 c4 = vec4(0,0,0,1);
	mat4 M = mat4(c1,c2,c3,c4);
	return M;
}

mat4 rotate(float angle,float x,float y,float z)
{
	float r = DOR(angle);
	mat4 M = mat4(1);

	vec4 c1 = vec4(cos(r)+(1-cos(r))*x*x,(1-cos(r))*y*x+sin(r)*z,(1-cos(r))*z*x-sin(r)*y,0);
	vec4 c2 = vec4((1-cos(r))*y*x-sin(r)*z,cos(r)+(1-cos(r))*y*y,(1-cos(r))*z*y+sin(r)*x,0);
	vec4 c3 = vec4((1-cos(r))*z*x+sin(r)*y,(1-cos(r))*z*y-sin(r)*x,cos(r)+(1-cos(r))*z*z,0);
	vec4 c4 = vec4(0,0,0,1);
	M = mat4(c1,c2,c3,c4);
	return M;
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case '1':
		/*angle += 5;
		if(angle>=360) angle = 0;
		printf("beta:%f\n",angle);*/
		rrr += 10;
		break;
	case '2':
		/*angle -= 5;
		if(angle<=0) angle = 360;
		printf("beta:%f\n",angle);*/
		rrr -= 10;
		break;
	case 'w':
		eyedistance -= 0.2;
		break;
	case 's':
		eyedistance += 0.2;
		break;
	case 'a':
		eyeAngley -=10;
		break;
	case 'd':
		eyeAngley +=10;
		break;
	case 'r':
		angles[1] -= 5; 
		if(angles[1] == -360) angles[1] = 0;
		movey = 0;
		movex = 0;
		break;
	case 't':
		angles[2] -= 5;
		if(angles[2] == -360) angles[2] = 0;
		movey = 0;
		movex = 0;
		break;
	case 'q':
		break;
	case 'e':
		break;
	}
	glutPostRedisplay();
}

void menuEvents(int option)
{
	frag = option;
}

void ActionMenuEvents(int option)
{
	switch(option)
	{
	case 0:
		action = IDLE;
		break;
	case 1:
		ActionIDLE();
		action = WALK;
		break;
	case 2:
		circle = 0;
		ActionIDLE();
		ActionOrder = 0;
		action = FLY;
		break;
	case 3:
		ActionOrder = 0;
		ActionIDLE();
		action = AIM;
		break;
	}
}

void ModeMenuEvents(int option)
{
	switch(option)
	{
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
}

void ShaderMenuEvents(int option)
{
	frag = option;
	/*if (frag == 5)
		texture = TextureApp::GenTexture("backgroundtoon.jpg");
	else
		texture = TextureApp::GenTexture("background.jpg");*/
}


