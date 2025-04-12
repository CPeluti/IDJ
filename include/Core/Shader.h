#include "SDL2/SDL_gpu.h"
#include <string>
#include <map>


class Shader {
    public:
        inline Shader():m_vert(0), m_frag(0), m_shader(0), m_uniforms(){}
        Shader(std::string vert_filename, std::string frag_filename);
        ~Shader();

        void Load(std::string vert, std::string frag);
        void Reset();

        int LoadLocation(std::string location);
        int GetLocation(std::string location);


        inline void Apply(){ 
            if(m_shader)
                GPU_ActivateShaderProgram(m_shader, &m_block);
            else  
                GPU_ActivateShaderProgram(0, NULL);
        }
        inline void Remove(){ GPU_ActivateShaderProgram(0, NULL); }

        inline int GetVert() {return m_vert;}
        inline int GetFrag() {return m_frag;}
        inline int GetShader() {return m_shader;}
        inline GPU_ShaderBlock* GetBlock() {return &m_block;}
    private:
        int m_vert, m_frag;
        int m_shader;
        GPU_ShaderBlock m_block;
        std::map<std::string, int> m_uniforms;

};