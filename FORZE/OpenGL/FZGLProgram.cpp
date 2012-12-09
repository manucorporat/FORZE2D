/*
 * FORZE ENGINE: http://forzefield.com
 *
 * Copyright (c) 2011-2012 FORZEFIELD Studios S.L.
 * Copyright (c) 2012 Manuel Martínez-Almeida
 * Copyright (c) 2008-2010 Ricardo Quesada
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 @author Manuel Martínez-Almeida
 */

#include "FZGLProgram.h"

#if FZ_GL_SHADERS

#include "FZGLState.h"
#include "FZMacros.h"
#include "FZResourcesManager.h"


using namespace STD;

namespace FORZE {
    
    typedef void (*GLInfoFunction)(GLuint program, 
                                   GLenum pname, 
                                   GLint* params);
    
    typedef void (*GLLogFunction) (GLuint program, 
                                   GLsizei bufsize, 
                                   GLsizei* length, 
                                   GLchar* infolog);
    
    
    static inline const char* shaderTypeToText(GLenum type)
    {
        if(type == GL_VERTEX_SHADER)
            return "VERTEX SHADER";
        else if(type == GL_FRAGMENT_SHADER)
            return "FRAGMENT SHADER";
        else
            return "";
    }
    
    
    static string logForOpenGLObject(GLuint object, GLInfoFunction infoFunc, GLLogFunction logFunc)
    {
        GLint logLength = 0, charsWritten = 0;
        
        infoFunc(object, GL_INFO_LOG_LENGTH, &logLength);   
        
        if (logLength < 1)
            return string();
        
        char *buffer = new char[logLength];
        logFunc(object, logLength, &charsWritten, buffer);
        string logOut((const char*)buffer, logLength);        
        delete [] buffer;
        
        CHECK_GL_ERROR_DEBUG();
        
        return logOut;
    }
    
    
#pragma mark - GLShader    
    
    GLShader::GLShader(const char *source, GLenum type)
    : m_shaderType(type), m_shader(0)
    {
        FZ_ASSERT(m_shaderType == GL_VERTEX_SHADER || m_shaderType == GL_FRAGMENT_SHADER, "Invalid shader type.");
        FZ_ASSERT(source != NULL, "Argumment must be non-NULL.");
        
        if(!compileShader(source))
            FZ_RAISE("GLProgram: Error compiling shader.");
    }
    
    
    GLShader::~GLShader()
    {
        if(m_shader)
            glDeleteShader(m_shader);
    }

    
    bool GLShader::compileShader(const char* source)
    {
        if(m_shader != 0)
            return true;
        
        m_shader = glCreateShader(m_shaderType);
        
        glShaderSource(m_shader, 1, &source, NULL);
        glCompileShader(m_shader);
        
        GLint status;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
        
        if(!status) {
            FZLOGERROR("GLShader: Error compiling %s.",shaderTypeToText(m_shaderType));
            FZLOGERROR("%s", getShaderLog().c_str());
        }
        CHECK_GL_ERROR_DEBUG();

        return !!status;
    }
    
    
    string GLShader::getShaderLog() const
    {
        if(!m_shader)
            FZLOGERROR("GLShader: Shader is not compiled yet.");
        else
            return logForOpenGLObject(m_shader, (GLInfoFunction)&glGetShaderiv, (GLLogFunction)&glGetShaderInfoLog);
        
        return string();
    }
    
    
#pragma mark - GLProgram
    
    GLProgram::GLProgram(const GLShader *shader1, const GLShader *shader2)
    : GLProgram(*shader1, *shader2)
    { }
    
    
    GLProgram::GLProgram(const GLShader& shader1, const GLShader& shader2)
    {
        FZ_ASSERT(shader1.getShaderType() != shader2.getShaderType(), "Shader's types must be different.");
        m_program = glCreateProgram();

        glAttachShader(m_program, shader1.getShader());
        glAttachShader(m_program, shader2.getShader());
        CHECK_GL_ERROR_DEBUG();
    }

    
    GLProgram::GLProgram(const char* vShaderFile, const char* fShaderFile)
    {
        m_program = glCreateProgram();
        CHECK_GL_ERROR_DEBUG();
        if(m_program == 0) {
            return;
        }
        
        
        // VERTEX SHADER
        fzBuffer source = ResourcesManager::Instance().loadResource(vShaderFile);
        GLShader vertexShader(source.getPointer(), GL_VERTEX_SHADER);
        source.free();
        
        
        // FRAGMENT SHADER (we can reuse the fzBuffer object)
        source = ResourcesManager::Instance().loadResource(fShaderFile);
        GLShader fragmentShader(source.getPointer(), GL_FRAGMENT_SHADER);
        source.free();
    
        
        // ATTACH SHADERS
        glAttachShader(m_program, vertexShader.getShader());
        glAttachShader(m_program, fragmentShader.getShader());

        CHECK_GL_ERROR_DEBUG();        
    }
    
    
    GLProgram::~GLProgram()
    {
        if (m_program)
            fzGLDeleteProgram(m_program);
    }
    
    
    void GLProgram::addAttribute(const char* attributeName, GLuint index)
    {
        glBindAttribLocation(m_program, index, attributeName);
        CHECK_GL_ERROR_DEBUG();
    }
    
    
    void GLProgram::addGenericAttributes()
    {
        addAttribute(kFZAttributeNamePosition, kFZAttribPosition);
        addAttribute(kFZAttributeNameColor, kFZAttribColor);
        addAttribute(kFZAttributeNameTexCoord, kFZAttribTexCoords);
    }
    
    
    bool GLProgram::link()
    {
        glLinkProgram(m_program);
        glValidateProgram(m_program);

        GLint status;
        glGetProgramiv(m_program, GL_LINK_STATUS, &status);

        if (status == GL_FALSE) {
            FZLOGERROR("GLProgram: Error linking program: %i.", m_program);
            FZLOGERROR("%s", getProgramLog().c_str());
            
            fzGLDeleteProgram( m_program );
            m_program = 0;
            return false;
        }

        char uniformName[256];
        GLint nuUniforms = 0;
        glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &nuUniforms);

        for(fzInt index = 0; index < nuUniforms; ++index)
        {
            glGetActiveUniform(m_program, index, 256, NULL, NULL, NULL, uniformName);
            m_uniforms.insert(uniformsPair(fzHash(uniformName), glGetUniformLocation(m_program, uniformName)));
        }
        CHECK_GL_ERROR_DEBUG();
        
        setUniform1i("u_texture", 0);

        return true;
    }
    
    
    void GLProgram::use() const
    {
        fzGLUseProgram(m_program);
    }
    
    
    GLint GLProgram::getUniform(const char* uniform) const
    {
        uniformsMap::const_iterator it(m_uniforms.find(fzHash(uniform)));
        if(it == m_uniforms.end())
            return -1;
        
        return it->second;
    }
    
    
    string GLProgram::getProgramLog()
    {
        return logForOpenGLObject(m_program,
                                  (GLInfoFunction)&glGetProgramiv,
                                  (GLLogFunction)&glGetProgramInfoLog
                                  );
    }
    
    
#pragma mark - Uniforms wrapper
    
    void GLProgram::setUniform1i(const char* uniform, int v1) const
    {
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform1i(location, v1);
            CHECK_GL_ERROR_DEBUG();
        }        
    }
    
    void GLProgram::setUniform1f(const char* uniform, float v1) const
    {
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform1f(location, v1);
            CHECK_GL_ERROR_DEBUG();
        }        
    }
    
    void GLProgram::setUniform2f(const char* uniform, float v1, float v2) const
    {
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform2f(location, v1, v2);
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    void GLProgram::setUniform3f(const char* uniform, float v1, float v2, float v3) const
    {
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform3f(location, v1, v2, v3);
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    void GLProgram::setUniform4f(const char* uniform, float v1, float v2, float v3, float v4) const
    {
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform4f(location, v1, v2, v3, v4);
            CHECK_GL_ERROR_DEBUG();
        }        
    }
    
    void GLProgram::setUniform4x4f(const char* uniform, GLsizei count, bool transpose, float *matrix) const
    {
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniformMatrix4fv(location, count, transpose, matrix);
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    void GLProgram::setUniform1iv(const char* uniform, GLsizei count, int* values) const
    {
        if(count == 0)
            return;
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform1iv(location, count, values);
            CHECK_GL_ERROR_DEBUG();
        }        
    }
    
    void GLProgram::setUniform1fv(const char* uniform, GLsizei count, float* values) const
    {
        if(count == 0)
            return;
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform1fv(location, count, values);
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    void GLProgram::setUniform2fv(const char* uniform, GLsizei count, float* values) const
    {
        if(count == 0)
            return;
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform2fv(location, count, values);
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    void GLProgram::setUniform3fv(const char* uniform, GLsizei count, float* values) const
    {
        if(count == 0)
            return;
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform3fv(location, count, values);
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    void GLProgram::setUniform4fv(const char* uniform, GLsizei count, float* values) const
    {
        if(count == 0)
            return;
        use();
        GLint location = getUniform(uniform);
        if(location != -1) {
            glUniform4fv(location, count, values);
            CHECK_GL_ERROR_DEBUG();
        }
    }
}
#endif
