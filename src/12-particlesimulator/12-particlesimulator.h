#ifndef __12_PARTICLESIMULATOR_H__
#define __12_PARTICLESIMULATOR_H__

#include "vapp.h"

constexpr int MAX_ATTRACTORS = 64;

class ComputeParticleSimulator : public VermilionApplication
{
private:
    // Compute program
    GLuint  compute_prog;

    GLint   dt_location;

    // Posisition and velocity buffers
    union
    {
    struct
    {
    GLuint position_buffer;
    GLuint velocity_buffer;
    };
    GLuint buffers[2];
    };

    // TBOs
    union
    {
    struct
    {
    GLuint position_tbo;
    GLuint velocity_tbo;
    };
    GLuint tbos[2];
    };

    // Attractor UBO
    GLuint  attractor_buffer;

    // Program, vao and vbo to render a full screen quad
    GLuint  render_prog;

    GLuint  render_vao;

    GLuint  render_vbo;

    // Mass of the attractors
    float attractor_masses[MAX_ATTRACTORS];

    float aspect_ratio;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __12_PARTICLESIMULATOR_H__ */
