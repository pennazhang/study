#include <stdio.h>
#include <unistd.h>

#include <directfb.h>

static IDirectFB *dfb = NULL;

static IDirectFBSurface *primary = NULL;

static int screen_width  = 0;
static int screen_height = 0;
static IDirectFBSurface *logo = NULL;
#define DFBCHECK(x...)                                         \
  {                                                            \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
      {                                                        \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBErrorFatal( #x, err );                         \
      }                                                        \
  }
int main (int argc, char **argv)
{
  int left, right, top, bottom, y, i;
  IDirectFBImageProvider *provider;
    
  DFBSurfaceDescription dsc;
  DFBCHECK (DirectFBInit (&argc, &argv));
  DFBCHECK (DirectFBCreate (&dfb));
  DFBCHECK (dfb->SetCooperativeLevel (dfb, DFSCL_FULLSCREEN));
  
  dsc.flags = DSDESC_CAPS;
  dsc.caps  = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
  DFBCHECK (dfb->CreateSurface( dfb, &dsc, &primary ));
  DFBCHECK (primary->GetSize (primary, &screen_width, &screen_height));
  
  DFBCHECK (dfb->CreateImageProvider (dfb, "/home/percy/DirectFB-examples-1.2.0/data/dfblogo.png", &provider));
  DFBCHECK (provider->GetSurfaceDescription (provider, &dsc));
  DFBCHECK (dfb->CreateSurface( dfb, &dsc, &logo ));
  DFBCHECK (provider->RenderTo (provider, logo, NULL));
  provider->Release (provider);
  for (i = -dsc.width; i < screen_width; i++)
  {
  	DFBCHECK (primary->FillRectangle (primary, 0, 0, screen_width, screen_height));
    DFBCHECK (primary->Blit (primary, logo, NULL, i, (screen_height - dsc.height) / 2));
    DFBCHECK (primary->Flip (primary, NULL, DSFLIP_WAITFORSYNC));
  }
  logo->Release (logo);


  DFBCHECK (primary->FillRectangle (primary, 0, 0, screen_width, screen_height));

  DFBCHECK (primary->SetColor (primary, 0x80, 0x80, 0xff, 0xff));
	
  for (y = 0; y< screen_height / 2; y++)
  {
  	left = y;
  	top = y;
  	right = screen_width - y;
  	bottom = screen_height - y;
    DFBCHECK (primary->DrawLine (primary, left, top, right, bottom));
  }
  
  for (y = 0; y < 50; y++)
  {
	  DFBCHECK (primary->SetColor (primary, 0xff - y, 0xa0 + y, 0xc0 - y, 0x0 + y));
	  DFBCHECK (primary->DrawRectangle (primary, 500 + y, 100 + y, 200 - 2 * y, 100 - 2 * y));
  }
  
  for (y = 0; y < 50; y++)
  {
 	 DFBCHECK (primary->SetColor (primary, 0x0 + 2 * y, 0xa0 + y, 0xff - 2 * y, 0x30 + y));
 	 DFBCHECK (primary->FillTriangle(primary, 200 + y, 300 + y, 100 + 2 * y, 500 - y, 300 + 2 * y, 300 + 2 *y));
  }
  DFBCHECK (primary->Flip (primary, NULL, 0));

  sleep (10);

  primary->Release( primary );
  dfb->Release( dfb );

  return 23;
}
