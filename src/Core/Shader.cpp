#include "Core/Shader.h"

GPU_ShaderBlock load_shader_program(int* p, const char* vertex_shader_file, const char* fragment_shader_file, int *v, int *f)
{
    *v = GPU_LoadShader(GPU_VERTEX_SHADER, vertex_shader_file);
    
    if(!v)
        GPU_LogError("Failed to load vertex shader (%s): %s\n", vertex_shader_file, GPU_GetShaderMessage());
    
    *f = GPU_LoadShader(GPU_FRAGMENT_SHADER, fragment_shader_file);
    
    if(!*f)
        GPU_LogError("Failed to load fragment shader (%s): %s\n", fragment_shader_file, GPU_GetShaderMessage());
    
    *p = GPU_LinkShaders(*v, *f);
    
    if(!*p)
    {
		GPU_ShaderBlock b = {-1, -1, -1, -1};
        GPU_LogError("Failed to link shader program (%s + %s): %s\n", vertex_shader_file, fragment_shader_file, GPU_GetShaderMessage());
        return b;
    }
    
	{
		GPU_ShaderBlock block = GPU_LoadShaderBlock(*p, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
		GPU_ActivateShaderProgram(*p, &block);

		return block;
	}
}

Shader::Shader(std::string vert, std::string frag){
    this->Load(vert, frag);
}

Shader::~Shader(){
    GPU_FreeShaderProgram(m_shader);
    GPU_FreeShader(m_vert);
    GPU_FreeShader(m_frag);
}

void Shader::Load(std::string vert, std::string frag){
    m_block = load_shader_program(&this->m_shader, vert.c_str(), frag.c_str(), &m_vert, &m_frag);
}

void Shader::Reset(){
    GPU_FreeShaderProgram(m_shader);
    GPU_FreeShader(m_vert);
    GPU_FreeShader(m_frag);
    
    m_vert = 0;
    m_frag = 0;
    m_shader = 0;
    m_uniforms.clear();
    
    m_block.position_loc = -1;
    m_block.texcoord_loc = -1;
    m_block.color_loc = -1;
    m_block.modelViewProjection_loc = -1;
}

int Shader::LoadLocation(std::string location){
    int loc = GPU_GetUniformLocation(m_shader, location.c_str());
    if(loc){
        m_uniforms.insert({location, loc});
    } else {
        return 0;
    }
    return loc;
}

int Shader::GetLocation(std::string location){
    if(m_uniforms.count(location)){
        return m_uniforms[location];
    }
    return LoadLocation(location);
}
