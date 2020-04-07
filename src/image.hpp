#include <vector>
#include <string>

extern float projectionMatrix_[16];

namespace james {

	class image {

	public:

		image() {
			textureId_ = 0;
			vertexId_ = 0;

			if (!shaderCreated_) {
				create_shader();
			}
		};

		~image() {
			if (textureId_) {
				glDeleteTextures(1, &textureId_);
				textureId_ = 0;
			}

			if (vertexId_) {
				glDeleteBuffers(1, &vertexId_);
				vertexId_ = 0;
			}
		};

		void load(const std::uint8_t * data, int width, int height) {

			width_ = width;
			height_ = height;

			if (!textureId_) {

				// setup the texture

				if (data) {
					glGenTextures(1, &textureId_);

					glBindTexture(GL_TEXTURE_2D, textureId_);

					//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				// setup the vertex data

				float w = (float)width;
				float h = (float)height;

				std::vector<float> v = { 0, 0, 0, 1, -2, -1,
																 w, 0, 0, 1,	1, -1,
																 0, h, 0, 1, -2,	1,
																 w, h, 0, 1,	1,	1 };

				// store the vertex on the gpu

				glGenBuffers(1, &vertexId_);
				glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
				glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_DYNAMIC_DRAW);

			} else {
				if (data) glBindTexture(GL_TEXTURE_2D, textureId_);
			}

			if (data) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
		};


		void set_size(float new_width, float new_height) {
			width_ = new_width;
			height_ = new_height;
		};

		void update(float r1, float r2, float i1, float i2) {

			std::vector<float> v = { 0,			0,			 0, 1, r1, i1,
															 width_, 0,			 0, 1, r2, i1,
															 0,			height_, 0, 1, r1, i2,
															 width_, height_, 0, 1, r2, i2 };

			// store the vertex on the gpu

			glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
			glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_DYNAMIC_DRAW);
		};


		void display(float xPos, float yPos, float iter1, float iter2) {

			glUseProgram(program_);

			glUniformMatrix4fv(uniformProj_, 1, GL_FALSE, projectionMatrix_);

			glUniform4f(uniformOffset_, xPos, yPos, 0.0, 0.0);
			glUniform2f(uniformScale_, 1.0, 1.0);
			glUniform4f(uniformRot_, 0.0, 0.0, 0.0, 0.0);
			glUniform2f(uniformIter_, iter1, iter2); //variable inputs

			glUniform1i(attribSample_, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
			glEnableVertexAttribArray(attribPosition_);
			glEnableVertexAttribArray(attribTexcoords_);

			glVertexAttribPointer(attribPosition_, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (const GLvoid *)0);
			glVertexAttribPointer(attribTexcoords_, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (const GLvoid *)(4 * sizeof(GLfloat)));

			glBindTexture(GL_TEXTURE_2D, textureId_);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glDisableVertexAttribArray(attribPosition_);
			glDisableVertexAttribArray(attribTexcoords_);
			glUseProgram(0);
		};

	private:

		bool create_shader()
		{
			GLint status;

			// build the vertex shader

			std::string prog = 
				"#version 100\n\
				attribute vec4 position; \
				attribute vec2 a_texCoord; \
				uniform mat4 proj; \
				uniform vec4 offset; \
				uniform vec2 scale; \
				uniform vec4 rot; \
				varying vec4 v_texCoord; \
				void main() \
				{ \
					mat4 a_scale = mat4(scale.x, 0.0, 0.0, 0.0, 0.0, scale.y, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0); \
					mat4 rotation = mat4(cos(rot.z), sin(rot.z), 0, 0,	-sin(rot.z), cos(rot.z), 0, 0,	0, 0, 1, 0,	0, 0, 0, 1); \
					vec4 t = rotation * position; \
					gl_Position = proj * ((a_scale * t) + offset); \
					v_texCoord = vec4(a_texCoord.x, a_texCoord.y, 0.0, 1.0); \
				} \
			";

			const char * c_str = prog.c_str();

			vertexShader_ = glCreateShader(GL_VERTEX_SHADER);

			glShaderSource(vertexShader_, 1, &c_str, NULL);
			glCompileShader(vertexShader_);

			glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &status);

			if (status == GL_FALSE) { return false; }

			// build the fragment shader
			
			prog = 
				"#version 100\n\
				varying highp vec4 v_texCoord; \
				uniform sampler2D s_texture; \
				uniform highp vec2 iter; \
				void main() \
				{ \
					highp float red = 0.0; \
					highp float r = v_texCoord.x; \
					highp float i = v_texCoord.y; \
					highp float max_iteration = iter.x * 0.2; \
					highp float blue = iter.y; \
					\
					highp float rr = 0.0; \
					highp float ii = 0.0; \
					\
					highp float minrr = 100.0; \
					highp float minii = 100.0; \
					\
					highp float timea = 0.0; \
					if (sin(max_iteration/200.0) > 0.707) {timea = (sin(max_iteration/200.0) - 0.707)*0.003;} \
					highp float timeb = 0.0; \
					if (sin(max_iteration/200.0) < -0.707) {timeb = (sin(max_iteration/200.0) + 0.707)*0.03;} \
					\
					highp float sm20 = sin(max_iteration/20.0); \
					highp float sm32 = sin(max_iteration/32.0); \
					\
					for (highp float iteration	= 1.0; iteration < 90.0; iteration++) { \
						highp float tempr = r*r - i*i + sm20*v_texCoord.x + timea*iteration + timeb*sin(iteration/100.0)*iteration; \
						i = 2.0 * r*i + sm32*v_texCoord.y; \
						r = tempr; \
						rr = r*r; ii = i*i; \
						highp float sqdist = ii + rr; \
						if (sqdist < blue) {blue = sqdist; red += (1.0 - red) * 0.1; } \
						if (blue == 0.0) {red = 1.0; break;} \
						if (iteration >= max_iteration) break; \
						if (rr < minrr) minrr = rr; \
						if (ii < minii) minii = ii; \
					} \
					\
					highp float a = sin(max_iteration/50.0); \
					highp float b = sin(max_iteration/50.0+1.0471975512); \
					highp float c = sin(max_iteration/50.0+2.09439510239); \
					\
					highp float col1 = r/1.5; \
					highp float col2 = ((5.0 + log(blue)) * 35.0)/255.0; \
					highp float col3 = red; \
					\
					highp float colr = minrr * sin(max_iteration/120.0); \
					colr = colr * colr * colr * 1000.0; \
					highp float coli = minii * sin(max_iteration/180.0); \
					coli = coli * coli * coli * 1000.0; \
					\
					gl_FragColor = vec4(a*col1+b*col2+c*col3+colr+coli, b*col1+c*col2+a*col3+colr+coli, c*col1+a*col2+b*col3+colr+coli, 1.0); \
				} \
			";

			/*
			const char * fragmentShaderProg[] = {
				"varying vec4 v_texCoord;\n", //range to calculate
				"uniform sampler2D s_texture;\n", //texture input
				"uniform vec4 iter;\n", //inputs
				"void main()\n",
				"{\n",
				"	float red = 0.0;\n", //these will be used in colouring (store values for colours)
				"	float green = 0.0;\n",
				"	float r = v_texCoord.x;\n", //coords to calculate over
				"	float i = v_texCoord.y;\n",
				"	float max_iteration = iter.x * 0.05;\n", //to depth
				"	float d1 = 0.0*sqrt(r*r +i*i)*iter.y/10.0;\n", //distance to each point
				"	float d2 = sqrt((r-1.0)*(r-1.0) + i*i)*(50.0);\n",//+0.9*iter.y/6.0)/2.;\n",
				"	float d3 = sqrt((r-iter.z)*(r-iter.z) + (i-iter[3])*(i-iter[3]))*50.0;\n",
				//"	float d1 = sqrt((r+iter.y/2000.0)*(r+iter.y/2000.0) +(i+iter.y/4000.0)*(i+iter.y/4000.0))*50.0;\n",
				//"	float d2 = sqrt((r-1.0)*(r-1.0) + i*i)*50.0;\n",
				//"	float d3 = sqrt((r-1.0)*(r-1.0) + (i-1.0)*(i-1.0))*50.0;\n",
				"	float c = (cos(d1)) + (cos(d2)) + (cos(d3));\n", //sum	vectors in 2d
				"	float s = (sin(d1)) + (sin(d2)) + (sin(d3));\n",
				//"	float c = cos(d1) + cos(d2);\n",
				//"	float s = sin(d1) + sin(d2);\n",
				"	float colour = (c*c + s*s)/4.0;\n", //calculate magnitude of resultant vector
				//"	float colour = (0.5 + 0.5*sqrt(c*c + s*s))/2.0;\n",
				"	gl_FragColor = vec4(2.0*(3.0*colour-2.0) - (3.0*colour-2.0)*(3.0*colour-2.0), 2.0*(2.0*colour-1.0) - (2.0*colour-1.0)*(2.0*colour-1.0), 2.0*colour - colour*colour, 1.0);\n", //colourise
				//"	gl_FragColor = vec4(4.0*colour - 4.0*colour*colour, 6.0*colour - 9.0*colour*colour, 2.0*colour - colour*colour, 1.0);\n",
				//"	gl_FragColor = vec4(colour*2.0-0.5, 0.75*colour+0.25, 2.0*colour - colour*colour, 1.0);\n",
				//"	gl_FragColor = vec4((sqrt((5.0 + log(colour)) * colour) * 120.0)/255.0, ((5.0 + log(colour)) * 35.0)/255.0, (colour*25.0)/255.0, 1.0);\n",
				"}\n"
			};

			len = 18;

			*/


			/*
			const char * fragmentShaderProg[] = { "varying vec4 v_texCoord;\n",
			"uniform sampler2D s_texture;\n",
			"void main()\n",
			"{\n",
			"	gl_FragColor = texture2DProj(s_texture, v_texCoord.xyw);\n",
			"}\n"
			};
			len = 6;
			*/

			c_str = prog.c_str();

			fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);

			glShaderSource(fragmentShader_, 1, &c_str, NULL);
			glCompileShader(fragmentShader_);

			glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &status);

			if (status == GL_FALSE) {printf("did not compile\n"); check_compiled(fragmentShader_); return false; }

			// link the program and store the entry points

			program_ = glCreateProgram();

			glAttachShader(program_, vertexShader_);
			glAttachShader(program_, fragmentShader_);

			glLinkProgram(program_);

			glGetProgramiv(program_, GL_LINK_STATUS, &status);

			if (status == GL_FALSE) { printf("did not link\n"); return false; }

			attribPosition_ = glGetAttribLocation(program_, "position");
			attribTexcoords_ = glGetAttribLocation(program_, "a_texCoord");
			attribSample_ = glGetUniformLocation(program_, "s_texture");
			uniformProj_ = glGetUniformLocation(program_, "proj");
			uniformOffset_ = glGetUniformLocation(program_, "offset");
			uniformScale_ = glGetUniformLocation(program_, "scale");
			uniformRot_ = glGetUniformLocation(program_, "rot");
			uniformIter_ = glGetUniformLocation(program_, "iter");

			glUniform1i(attribSample_, 0); // try having this here rather than for each texture

			shaderCreated_ = true;

			return true;
		};

		static int check_compiled(GLuint shader) {

			GLint success = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (success == GL_FALSE) {
				GLint max_len = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_len);

				GLchar err_log[max_len];
				glGetShaderInfoLog(shader, max_len, &max_len, &err_log[0]);
				glDeleteShader(shader);

				printf("Shader compilation failed: %s\n", err_log);
			}

			return success;
		};


		static int check_linked(GLuint program) {

			GLint success = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &success);

			if (success == GL_FALSE) {
				GLint max_len = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len);

				GLchar err_log[max_len];
				glGetProgramInfoLog(program, max_len, &max_len, &err_log[0]);

				printf("Program linking failed: %s\n", err_log);
			}

			return success;
		}


		// Store the opengl texture id

		GLuint textureId_;

		// Store the opengl vertex id

		GLuint vertexId_;

		float width_;
		float height_;

		// Shader setings

		inline static bool shaderCreated_ = false;

		inline static GLuint program_ = 0;

		inline static GLuint vertexShader_ = 0;
		inline static GLuint fragmentShader_ = 0;

		inline static GLint	attribPosition_ = 0;
		inline static GLint	attribTexcoords_ = 0;
		inline static GLint	attribSample_ = 0;

		inline static GLint	uniformProj_ = 0;
		inline static GLint	uniformOffset_ = 0;
		inline static GLint	uniformScale_ = 0;
		inline static GLint	uniformRot_ = 0;
		inline static GLint	uniformIter_ = 0;

	}; // class image


} // namespace
