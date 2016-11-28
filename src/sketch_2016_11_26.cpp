
#include "sketches.h"

#ifdef SKETCH_2016_11_26

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_PARTICLES 100

class sketch_2016_11_26 : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
    void resize() override;
	void draw() override;
    
    cairo::SurfaceImage m_OnscreenBuffer;
    cairo::SurfaceImage m_OffscreenBuffer;
    cairo::Context      m_OffscreenContext;
    
    void render(cairo::Context &c);
    
    Perlin m_PerlinX[NUM_PARTICLES], m_PerlinY[NUM_PARTICLES], m_Perlin;
    vec2 m_Positions[NUM_PARTICLES];
    
    void reset();
    
    
};

void sketch_2016_11_26::setup()
{
    std::time_t seconds = std::time(0);
    m_Perlin = Perlin(4, seconds);
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        m_PerlinX[i] = Perlin(4, seconds * i);
        m_PerlinY[i] = Perlin(4, sqrt(seconds) * i);
    }

    
    reset();
    
 
}

void sketch_2016_11_26::reset()
{
    for(int i=0; i < NUM_PARTICLES; i++)
    {
        m_Positions[i] = getWindowCenter();
    }
    
    m_OnscreenBuffer   = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenBuffer  = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenContext = cairo::Context(m_OffscreenBuffer);
    

}

void sketch_2016_11_26::mouseDown( MouseEvent event )
{
    reset();
}

void sketch_2016_11_26::resize()
{
    m_OnscreenBuffer   = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenBuffer  = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenContext = cairo::Context(m_OffscreenBuffer);
}

void sketch_2016_11_26::update()
{
    
    float stepSize = getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth();
    float stepX = 0;
    float stepY = 0;
    
    float timeDivisor = 1.0f;
    float freq = 1 / 20.0;
    
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        float t = getElapsedSeconds()/timeDivisor;
        
        float px = m_Perlin.fBm(vec2(i, t) * freq + 1.0f);
        float py = m_Perlin.fBm(vec2(i + NUM_PARTICLES, t) * freq + 1.0f);
        
        stepX = px;
        stepY = py;

        m_Positions[i] = vec2(stepX*stepSize, stepY*stepSize) + getWindowCenter();
    }
}

void sketch_2016_11_26::draw()
{
	gl::clear( Color( 1, 1, 1 ) );
    

    
    render(m_OffscreenContext);
    
    cairo::Context c(m_OnscreenBuffer);
    c.copySurface(m_OffscreenBuffer, m_OffscreenBuffer.getBounds());
    
    gl::draw( gl::Texture2d::create(m_OnscreenBuffer.getSurface()) );
    
}



void sketch_2016_11_26::render(cairo::Context &c)
{
    float distThresh = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.05;
    float alpha = 1.0;
    CAIRO_OPERATOR_DARKEN;
    c.setOperator(CAIRO_OPERATOR_DARKEN);
    c.setSource(ColorA(0,0,0,0.1));
    c.paint();
    
    c.setSource(ColorA(1,1,1,1));
    
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        c.circle(m_Positions[i], 2);
        c.fill();
    }
    
    c.setSource(ColorA(1,1,1,alpha));
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        float dist = 0.0;
        vec2 start = m_Positions[i];
        
        for(int j = i+1; j < NUM_PARTICLES; j++)
        {
            dist = distance(start, m_Positions[j]);
            
            if(dist < distThresh)
            {
                c.setLineWidth(0.5);
                c.line(start, m_Positions[j]);
                c.stroke();
            }
        }
    }
    
}

CINDER_APP( sketch_2016_11_26, RendererGl )

#endif
