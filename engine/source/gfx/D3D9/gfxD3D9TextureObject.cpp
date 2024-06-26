//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// 
// Copyright (c) 2003 GarageGames.Com
//-----------------------------------------------------------------------------
#include "gfx/D3D9/gfxD3D9Device.h"
#include "gfx/D3D9/gfxD3D9TextureObject.h"
#include "platform/profiler.h"

#ifdef TORQUE_OS_XENON
#  include "gfx/D3D9/360/gfx360Device.h"
#endif

U32 GFXD3D9TextureObject::mTexCount = 0;

//*****************************************************************************
// GFX D3D Texture Object
//*****************************************************************************
GFXD3D9TextureObject::GFXD3D9TextureObject( GFXDevice * d, GFXTextureProfile *profile)
                                        : GFXTextureObject( d, profile )
{
#ifdef D3D_TEXTURE_SPEW
   mTexCount++;
   Con::printf("+ texMake %d %x", mTexCount, this);
#endif

   isManaged = false;
   mD3DTexture = NULL;
   mLocked = false;

   mD3DSurface = NULL;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GFXD3D9TextureObject::~GFXD3D9TextureObject()
{
   kill();
#ifdef D3D_TEXTURE_SPEW
   mTexCount--;
   Con::printf("+ texkill %d %x", mTexCount, this);
#endif
}

//-----------------------------------------------------------------------------
// lock
//-----------------------------------------------------------------------------
GFXLockedRect *GFXD3D9TextureObject::lock(U32 mipLevel /*= 0*/, RectI *inRect /*= NULL*/)
{
   LPDIRECT3DDEVICE9 D3DDevice = static_cast<GFXD3D9Device *>(GFX)->getDevice();
   
#ifdef TORQUE_OS_XENON
   // Un-set the texture if it is currently set in the device
   if( get2DTex()->IsSet( D3DDevice ) )
      reinterpret_cast<GFX360Device *>( GFX )->clearTextureHolds();
#endif

   if( mProfile->isRenderTarget() )
   {
      if( !mLockTex || 
          mLockTex->getWidth() != getWidth() ||
          mLockTex->getHeight() != getHeight() )
      {
         mLockTex.set( getWidth(), getHeight(), mFormat, &GFXSystemMemProfile );
      }

      PROFILE_START(GFXD3D9TextureObject_lockRT);
      
      GFXD3D9TextureObject *to = (GFXD3D9TextureObject *) &(*mLockTex);

      IDirect3DSurface9 *dest;
      D3D9Assert( to->get2DTex()->GetSurfaceLevel( 0, &dest ), "GFXD3D9TextureObject::lock - failed to get dest texture's surface." );

#ifdef TORQUE_OS_XENON
      D3DDevice->Resolve( D3DRESOLVE_RENDERTARGET0, NULL, to->get2DTex(), NULL, 0, 0, NULL, NULL, NULL, NULL );
#else
      IDirect3DSurface9 *source;
      D3D9Assert( get2DTex()->GetSurfaceLevel( 0, &source ), "GFXD3D9TextureObject::lock - failed to get our own texture's surface." );

      HRESULT rtLockRes = D3DDevice->GetRenderTargetData( source, dest );

      source->Release();

      if(!SUCCEEDED(rtLockRes))
      {
         // This case generally occurs if the device is lost. The lock failed
         // so clean up and return NULL.
         dest->Release();
         PROFILE_END();
         return NULL;
      }
#endif

      D3D9Assert( dest->LockRect( &mLockRect, NULL, D3DLOCK_READONLY ), NULL );
      dest->Release();
      mLocked = true;

      PROFILE_END();
   }
   else
   {
      RECT r;

      if(inRect)
      {
         r.top  = inRect->point.y;
         r.left = inRect->point.x;
         r.bottom = inRect->point.y + inRect->extent.y;
         r.right  = inRect->point.x + inRect->extent.x;
      }

      D3D9Assert( get2DTex()->LockRect(mipLevel, &mLockRect, inRect ? &r : NULL, 0), 
         "GFXD3D9TextureObject::lock - could not lock non-RT texture!" );
      mLocked = true;

   }

   // GFXLockedRect is set up to correspond to D3DLOCKED_RECT, so this is ok.
   return (GFXLockedRect*)&mLockRect; 
}
   
//-----------------------------------------------------------------------------
// unLock
//-----------------------------------------------------------------------------
void GFXD3D9TextureObject::unlock(U32 mipLevel)
{
   AssertFatal( mLocked, "Attempting to unlock a surface that has not been locked" );

#ifndef TORQUE_OS_XENON
   if( mProfile->isRenderTarget() )
   {
      IDirect3DSurface9 *dest;
      GFXD3D9TextureObject *to = (GFXD3D9TextureObject *) &(*mLockTex);
      D3D9Assert( to->get2DTex()->GetSurfaceLevel( 0, &dest ), NULL );

      dest->UnlockRect();
      dest->Release();

      mLocked = false;
   }
   else
#endif
   {
      D3D9Assert( get2DTex()->UnlockRect(mipLevel), 
         "GFXD3D9TextureObject::unlock - could not unlock non-RT texture." );

      mLocked = false;
   }
}

//------------------------------------------------------------------------------

void GFXD3D9TextureObject::release()
{
   static_cast<GFXD3D9Device *>( GFX )->destroyD3DResource( mD3DTexture );
   static_cast<GFXD3D9Device *>( GFX )->destroyD3DResource( mD3DSurface );
   mD3DTexture = NULL;
   mD3DSurface = NULL;
}

void GFXD3D9TextureObject::zombify()
{
   // Managed textures are managed by D3D
   AssertFatal(!mLocked, "GFXD3D9TextureObject::zombify - Cannot zombify a locked texture!");
   if(isManaged)
      return;

   release();
}

void GFXD3D9TextureObject::resurrect()
{
   // Managed textures are managed by D3D
   if(isManaged)
      return;

   static_cast<GFXD3D9TextureManager*>(GFX->getTextureManager())->refreshTexture(this);
}

//------------------------------------------------------------------------------

bool GFXD3D9TextureObject::copyToBmp(GBitmap* bmp)
{
#ifdef TORQUE_OS_XENON
   // TODO: Implement Xenon version -patw
   return false;
#else
   if (!bmp)
      return false;

   AssertFatal(mProfile->isRenderTarget(), "copyToBmp: this texture is not a render target");
   if (!mProfile->isRenderTarget())
      return false;

   // check format limitations
   // at the moment we only support RGBA for the source (other 4 byte formats should
   // be easy to add though)
   AssertFatal(mFormat == GFXFormatR8G8B8A8, "copyToBmp: invalid format");
   if (mFormat != GFXFormatR8G8B8A8)
      return false;

   PROFILE_START(GFXD3D9TextureObject_copyToBmp);

   AssertFatal(bmp->getWidth() == getWidth(), "doh");
   AssertFatal(bmp->getHeight() == getHeight(), "doh");
   U32 width = getWidth();
   U32 height = getHeight();

   // set some constants
   const U32 sourceBytesPerPixel = 4;
   U32 destBytesPerPixel = 0;
   if (bmp->getFormat() == GFXFormatR8G8B8A8)
      destBytesPerPixel = 4;
   else if (bmp->getFormat() == GFXFormatR8G8B8)
      destBytesPerPixel = 3;
   else
      // unsupported
      AssertFatal(false, "unsupported bitmap format");

   // lock the texture
   D3DLOCKED_RECT* lockRect = (D3DLOCKED_RECT*) lock();

   // set pointers
   U8* srcPtr = (U8*)lockRect->pBits;
   U8* destPtr = bmp->getWritableBits();

   // we will want to skip over any D3D cache data in the source texture
   const S32 sourceCacheSize = lockRect->Pitch - width * sourceBytesPerPixel;
   AssertFatal(sourceCacheSize >= 0, "copyToBmp: cache size is less than zero?");

   PROFILE_START(GFXD3D9TextureObject_copyToBmp_pixCopy);
   // copy data into bitmap
   for (int row = 0; row < height; ++row)
   {
      for (int col = 0; col < width; ++col)
      {
         destPtr[0] = srcPtr[2]; // red
         destPtr[1] = srcPtr[1]; // green
         destPtr[2] = srcPtr[0]; // blue 
         if (destBytesPerPixel == 4)
            destPtr[3] = srcPtr[3]; // alpha

         // go to next pixel in src
         srcPtr += sourceBytesPerPixel;

         // go to next pixel in dest
         destPtr += destBytesPerPixel;
      }
      // skip past the cache data for this row (if any)
      srcPtr += sourceCacheSize;
   }
   PROFILE_END();

   // assert if we stomped or underran memory
   AssertFatal(U32(destPtr - bmp->getWritableBits()) == width * height * destBytesPerPixel, "copyToBmp: doh, memory error");
   AssertFatal(U32(srcPtr - (U8*)lockRect->pBits) == height * lockRect->Pitch, "copyToBmp: doh, memory error");

   // unlock
   unlock();

   PROFILE_END();

   return true;
#endif
}

//------------------------------------------------------------------------------

bool GFXD3D9TextureObject::readBackBuffer( Point2I &upperLeft )
{
#ifdef TORQUE_OS_XENON
   // TODO: Implement Xenon version -patw
   return false;
#else
   AssertFatal( mProfile->isRenderTarget(), "readBackBuffer: this texture is not a render target" );
   if ( !mProfile->isRenderTarget() )
      return false;

   GFXD3D9Device *device = dynamic_cast<GFXD3D9Device *>( mDevice );

   AssertFatal( device != NULL, "readBackBuffer: This is totally not a D3D9 device." );

   IDirect3DSurface9 *backBuffer;
   device->mD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer );

   IDirect3DSurface9 *surf;

   // TODO: Find out why this doesn't work correctly -pw
   RECT rect;
   rect.top = upperLeft.y;
   rect.left = upperLeft.x;
   rect.bottom = rect.top + getHeight();
   rect.right = rect.left + getWidth();

   get2DTex()->GetSurfaceLevel( 0, &surf );
   device->mD3DDevice->StretchRect( backBuffer, NULL, surf, NULL, D3DTEXF_NONE );

   surf->Release();
   backBuffer->Release();

   return true;
#endif
}
