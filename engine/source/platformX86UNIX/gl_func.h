//-----------------------------------------------------------------------------
// Torque Shader Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------



// Most of this is from...
/*
 * Mesa 3-D graphics library
 * Version:  3.4
 *
 * Copyright (C) 1999-2000  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

//------------------------------------------------------------------------------
// GL functions
//------------------------------------------------------------------------------

/*
 * Miscellaneous
 */

GL_FUNCTION(void, glClearIndex, ( GLfloat c ), return; )

GL_FUNCTION(void, glClearColor, ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ), return; )

GL_FUNCTION(void, glClear, ( GLbitfield mask ), return; )

GL_FUNCTION(void, glIndexMask, ( GLuint mask ), return; )

GL_FUNCTION(void, glColorMask, ( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ), return; )

GL_FUNCTION(void, glAlphaFunc, ( GLenum func, GLclampf ref ), return; )

GL_FUNCTION(void, glBlendFunc, ( GLenum sfactor, GLenum dfactor ), return; )

GL_FUNCTION(void, glLogicOp, ( GLenum opcode ), return; )

GL_FUNCTION(void, glCullFace, ( GLenum mode ), return; )

GL_FUNCTION(void, glFrontFace, ( GLenum mode ), return; )

GL_FUNCTION(void, glPointSize, ( GLfloat size ), return; )

GL_FUNCTION(void, glLineWidth, ( GLfloat width ), return; )

GL_FUNCTION(void, glLineStipple, ( GLint factor, GLushort pattern ), return; )

GL_FUNCTION(void, glPolygonMode, ( GLenum face, GLenum mode ), return; )

GL_FUNCTION(void, glPolygonOffset, ( GLfloat factor, GLfloat units ), return; )

GL_FUNCTION(void, glPolygonStipple, ( const GLubyte *mask ), return; )

GL_FUNCTION(void, glGetPolygonStipple, ( GLubyte *mask ), return; )

GL_FUNCTION(void, glEdgeFlag, ( GLboolean flag ), return; )

GL_FUNCTION(void, glEdgeFlagv, ( const GLboolean *flag ), return; )

GL_FUNCTION(void, glScissor, ( GLint x, GLint y, GLsizei width, GLsizei height), return; )

GL_FUNCTION(void, glClipPlane, ( GLenum plane, const GLdouble *equation ), return; )

GL_FUNCTION(void, glGetClipPlane, ( GLenum plane, GLdouble *equation ), return; )

GL_FUNCTION(void, glDrawBuffer, ( GLenum mode ), return; )

GL_FUNCTION(void, glReadBuffer, ( GLenum mode ), return; )

GL_FUNCTION(void, glEnable, ( GLenum cap ), return; )

GL_FUNCTION(void, glDisable, ( GLenum cap ), return; )

GL_FUNCTION(GLboolean, glIsEnabled, ( GLenum cap ), return GL_FALSE; )


GL_FUNCTION(void, glEnableClientState, ( GLenum cap ), return; )  /* 1.1 */

GL_FUNCTION(void, glDisableClientState, ( GLenum cap ), return; )  /* 1.1 */


GL_FUNCTION(void, glGetBooleanv, ( GLenum pname, GLboolean *params ), return; )

GL_FUNCTION(void, glGetDoublev, ( GLenum pname, GLdouble *params ), return; )

GL_FUNCTION(void, glGetFloatv, ( GLenum pname, GLfloat *params ), return; )

GL_FUNCTION(void, glGetIntegerv, ( GLenum pname, GLint *params ), return; )


GL_FUNCTION(void, glPushAttrib, ( GLbitfield mask ), return; )

GL_FUNCTION(void, glPopAttrib, ( void ), return; )


GL_FUNCTION(void, glPushClientAttrib, ( GLbitfield mask ), return; )  /* 1.1 */

GL_FUNCTION(void, glPopClientAttrib, ( void ), return; )  /* 1.1 */


GL_FUNCTION(GLint, glRenderMode, ( GLenum mode ), return 0; )

GL_FUNCTION(GLenum, glGetError, ( void ), return 0; )

GL_FUNCTION(const GLubyte*, glGetString, ( GLenum name ), return (const GLubyte*)""; )

GL_FUNCTION(void, glFinish, ( void ), return; )

GL_FUNCTION(void, glFlush, ( void ), return; )

GL_FUNCTION(void, glHint, ( GLenum target, GLenum mode ), return; )



/*
 * Depth Buffer
 */

GL_FUNCTION(void, glClearDepth, ( GLclampd depth ), return; )

GL_FUNCTION(void, glDepthFunc, ( GLenum func ), return; )

GL_FUNCTION(void, glDepthMask, ( GLboolean flag ), return; )

GL_FUNCTION(void, glDepthRange, ( GLclampd near_val, GLclampd far_val ), return; )


/*
 * Accumulation Buffer
 */

GL_FUNCTION(void, glClearAccum, ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ), return; )

GL_FUNCTION(void, glAccum, ( GLenum op, GLfloat value ), return; )



/*
 * Transformation
 */

GL_FUNCTION(void, glMatrixMode, ( GLenum mode ), return; )

GL_FUNCTION(void, glOrtho, ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ), return; )

GL_FUNCTION(void, glFrustum, ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ), return; )

GL_FUNCTION(void, glViewport, ( GLint x, GLint y, GLsizei width, GLsizei height ), return; )

GL_FUNCTION(void, glPushMatrix, ( void ), return; )

GL_FUNCTION(void, glPopMatrix, ( void ), return; )

GL_FUNCTION(void, glLoadIdentity, ( void ), return; )

GL_FUNCTION(void, glLoadMatrixd, ( const GLdouble *m ), return; )
GL_FUNCTION(void, glLoadMatrixf, ( const GLfloat *m ), return; )

GL_FUNCTION(void, glMultMatrixd, ( const GLdouble *m ), return; )
GL_FUNCTION(void, glMultMatrixf, ( const GLfloat *m ), return; )

GL_FUNCTION(void, glRotated, ( GLdouble angle, GLdouble x, GLdouble y, GLdouble z ), return; )
GL_FUNCTION(void, glRotatef, ( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ), return; )

GL_FUNCTION(void, glScaled, ( GLdouble x, GLdouble y, GLdouble z ), return; )
GL_FUNCTION(void, glScalef, ( GLfloat x, GLfloat y, GLfloat z ), return; )

GL_FUNCTION(void, glTranslated, ( GLdouble x, GLdouble y, GLdouble z ), return; )
GL_FUNCTION(void, glTranslatef, ( GLfloat x, GLfloat y, GLfloat z ), return; )



/*
 * Display Lists
 */

GL_FUNCTION(GLboolean, glIsList, ( GLuint list ), return GL_FALSE; )

GL_FUNCTION(void, glDeleteLists, ( GLuint list, GLsizei range ), return; )

GL_FUNCTION(GLuint, glGenLists, ( GLsizei range ), return 0; )

GL_FUNCTION(void, glNewList, ( GLuint list, GLenum mode ), return; )

GL_FUNCTION(void, glEndList, ( void ), return; )

GL_FUNCTION(void, glCallList, ( GLuint list ), return; )

GL_FUNCTION(void, glCallLists, ( GLsizei n, GLenum type, const GLvoid *lists ), return; )

GL_FUNCTION(void, glListBase, ( GLuint base ), return; )



/*
 * Drawing Functions
 */

GL_FUNCTION(void, glBegin, ( GLenum mode ), return; )

GL_FUNCTION(void, glEnd, ( void ), return; )


GL_FUNCTION(void, glVertex2d, ( GLdouble x, GLdouble y ), return; )
GL_FUNCTION(void, glVertex2f, ( GLfloat x, GLfloat y ), return; )
GL_FUNCTION(void, glVertex2i, ( GLint x, GLint y ), return; )
GL_FUNCTION(void, glVertex2s, ( GLshort x, GLshort y ), return; )

GL_FUNCTION(void, glVertex3d, ( GLdouble x, GLdouble y, GLdouble z ), return; )
GL_FUNCTION(void, glVertex3f, ( GLfloat x, GLfloat y, GLfloat z ), return; )
GL_FUNCTION(void, glVertex3i, ( GLint x, GLint y, GLint z ), return; )
GL_FUNCTION(void, glVertex3s, ( GLshort x, GLshort y, GLshort z ), return; )

GL_FUNCTION(void, glVertex4d, ( GLdouble x, GLdouble y, GLdouble z, GLdouble w ), return; )
GL_FUNCTION(void, glVertex4f, ( GLfloat x, GLfloat y, GLfloat z, GLfloat w ), return; )
GL_FUNCTION(void, glVertex4i, ( GLint x, GLint y, GLint z, GLint w ), return; )
GL_FUNCTION(void, glVertex4s, ( GLshort x, GLshort y, GLshort z, GLshort w ), return; )

GL_FUNCTION(void, glVertex2dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glVertex2fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glVertex2iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glVertex2sv, ( const GLshort *v ), return; )

GL_FUNCTION(void, glVertex3dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glVertex3fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glVertex3iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glVertex3sv, ( const GLshort *v ), return; )

GL_FUNCTION(void, glVertex4dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glVertex4fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glVertex4iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glVertex4sv, ( const GLshort *v ), return; )


GL_FUNCTION(void, glNormal3b, ( GLbyte nx, GLbyte ny, GLbyte nz ), return; )
GL_FUNCTION(void, glNormal3d, ( GLdouble nx, GLdouble ny, GLdouble nz ), return; )
GL_FUNCTION(void, glNormal3f, ( GLfloat nx, GLfloat ny, GLfloat nz ), return; )
GL_FUNCTION(void, glNormal3i, ( GLint nx, GLint ny, GLint nz ), return; )
GL_FUNCTION(void, glNormal3s, ( GLshort nx, GLshort ny, GLshort nz ), return; )

GL_FUNCTION(void, glNormal3bv, ( const GLbyte *v ), return; )
GL_FUNCTION(void, glNormal3dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glNormal3fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glNormal3iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glNormal3sv, ( const GLshort *v ), return; )


GL_FUNCTION(void, glIndexd, ( GLdouble c ), return; )
GL_FUNCTION(void, glIndexf, ( GLfloat c ), return; )
GL_FUNCTION(void, glIndexi, ( GLint c ), return; )
GL_FUNCTION(void, glIndexs, ( GLshort c ), return; )
GL_FUNCTION(void, glIndexub, ( GLubyte c ), return; )  /* 1.1 */

GL_FUNCTION(void, glIndexdv, ( const GLdouble *c ), return; )
GL_FUNCTION(void, glIndexfv, ( const GLfloat *c ), return; )
GL_FUNCTION(void, glIndexiv, ( const GLint *c ), return; )
GL_FUNCTION(void, glIndexsv, ( const GLshort *c ), return; )
GL_FUNCTION(void, glIndexubv, ( const GLubyte *c ), return; )  /* 1.1 */

GL_FUNCTION(void, glColor3b, ( GLbyte red, GLbyte green, GLbyte blue ), return; )
GL_FUNCTION(void, glColor3d, ( GLdouble red, GLdouble green, GLdouble blue ), return; )
GL_FUNCTION(void, glColor3f, ( GLfloat red, GLfloat green, GLfloat blue ), return; )
GL_FUNCTION(void, glColor3i, ( GLint red, GLint green, GLint blue ), return; )
GL_FUNCTION(void, glColor3s, ( GLshort red, GLshort green, GLshort blue ), return; )
GL_FUNCTION(void, glColor3ub, ( GLubyte red, GLubyte green, GLubyte blue ), return; )
GL_FUNCTION(void, glColor3ui, ( GLuint red, GLuint green, GLuint blue ), return; )
GL_FUNCTION(void, glColor3us, ( GLushort red, GLushort green, GLushort blue ), return; )

GL_FUNCTION(void, glColor4b, ( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha ), return; )
GL_FUNCTION(void, glColor4d, ( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha ), return; )
GL_FUNCTION(void, glColor4f, ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ), return; )
GL_FUNCTION(void, glColor4i, ( GLint red, GLint green, GLint blue, GLint alpha ), return; )
GL_FUNCTION(void, glColor4s, ( GLshort red, GLshort green, GLshort blue, GLshort alpha ), return; )
GL_FUNCTION(void, glColor4ub, ( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha ), return; )
GL_FUNCTION(void, glColor4ui, ( GLuint red, GLuint green, GLuint blue, GLuint alpha ), return; )
GL_FUNCTION(void, glColor4us, ( GLushort red, GLushort green, GLushort blue, GLushort alpha ), return; )


GL_FUNCTION(void, glColor3bv, ( const GLbyte *v ), return; )
GL_FUNCTION(void, glColor3dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glColor3fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glColor3iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glColor3sv, ( const GLshort *v ), return; )
GL_FUNCTION(void, glColor3ubv, ( const GLubyte *v ), return; )
GL_FUNCTION(void, glColor3uiv, ( const GLuint *v ), return; )
GL_FUNCTION(void, glColor3usv, ( const GLushort *v ), return; )

GL_FUNCTION(void, glColor4bv, ( const GLbyte *v ), return; )
GL_FUNCTION(void, glColor4dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glColor4fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glColor4iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glColor4sv, ( const GLshort *v ), return; )
GL_FUNCTION(void, glColor4ubv, ( const GLubyte *v ), return; )
GL_FUNCTION(void, glColor4uiv, ( const GLuint *v ), return; )
GL_FUNCTION(void, glColor4usv, ( const GLushort *v ), return; )


GL_FUNCTION(void, glTexCoord1d, ( GLdouble s ), return; )
GL_FUNCTION(void, glTexCoord1f, ( GLfloat s ), return; )
GL_FUNCTION(void, glTexCoord1i, ( GLint s ), return; )
GL_FUNCTION(void, glTexCoord1s, ( GLshort s ), return; )

GL_FUNCTION(void, glTexCoord2d, ( GLdouble s, GLdouble t ), return; )
GL_FUNCTION(void, glTexCoord2f, ( GLfloat s, GLfloat t ), return; )
GL_FUNCTION(void, glTexCoord2i, ( GLint s, GLint t ), return; )
GL_FUNCTION(void, glTexCoord2s, ( GLshort s, GLshort t ), return; )

GL_FUNCTION(void, glTexCoord3d, ( GLdouble s, GLdouble t, GLdouble r ), return; )
GL_FUNCTION(void, glTexCoord3f, ( GLfloat s, GLfloat t, GLfloat r ), return; )
GL_FUNCTION(void, glTexCoord3i, ( GLint s, GLint t, GLint r ), return; )
GL_FUNCTION(void, glTexCoord3s, ( GLshort s, GLshort t, GLshort r ), return; )

GL_FUNCTION(void, glTexCoord4d, ( GLdouble s, GLdouble t, GLdouble r, GLdouble q ), return; )
GL_FUNCTION(void, glTexCoord4f, ( GLfloat s, GLfloat t, GLfloat r, GLfloat q ), return; )
GL_FUNCTION(void, glTexCoord4i, ( GLint s, GLint t, GLint r, GLint q ), return; )
GL_FUNCTION(void, glTexCoord4s, ( GLshort s, GLshort t, GLshort r, GLshort q ), return; )

GL_FUNCTION(void, glTexCoord1dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glTexCoord1fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glTexCoord1iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glTexCoord1sv, ( const GLshort *v ), return; )

GL_FUNCTION(void, glTexCoord2dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glTexCoord2fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glTexCoord2iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glTexCoord2sv, ( const GLshort *v ), return; )

GL_FUNCTION(void, glTexCoord3dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glTexCoord3fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glTexCoord3iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glTexCoord3sv, ( const GLshort *v ), return; )

GL_FUNCTION(void, glTexCoord4dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glTexCoord4fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glTexCoord4iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glTexCoord4sv, ( const GLshort *v ), return; )


GL_FUNCTION(void, glRasterPos2d, ( GLdouble x, GLdouble y ), return; )
GL_FUNCTION(void, glRasterPos2f, ( GLfloat x, GLfloat y ), return; )
GL_FUNCTION(void, glRasterPos2i, ( GLint x, GLint y ), return; )
GL_FUNCTION(void, glRasterPos2s, ( GLshort x, GLshort y ), return; )

GL_FUNCTION(void, glRasterPos3d, ( GLdouble x, GLdouble y, GLdouble z ), return; )
GL_FUNCTION(void, glRasterPos3f, ( GLfloat x, GLfloat y, GLfloat z ), return; )
GL_FUNCTION(void, glRasterPos3i, ( GLint x, GLint y, GLint z ), return; )
GL_FUNCTION(void, glRasterPos3s, ( GLshort x, GLshort y, GLshort z ), return; )

GL_FUNCTION(void, glRasterPos4d, ( GLdouble x, GLdouble y, GLdouble z, GLdouble w ), return; )
GL_FUNCTION(void, glRasterPos4f, ( GLfloat x, GLfloat y, GLfloat z, GLfloat w ), return; )
GL_FUNCTION(void, glRasterPos4i, ( GLint x, GLint y, GLint z, GLint w ), return; )
GL_FUNCTION(void, glRasterPos4s, ( GLshort x, GLshort y, GLshort z, GLshort w ), return; )

GL_FUNCTION(void, glRasterPos2dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glRasterPos2fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glRasterPos2iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glRasterPos2sv, ( const GLshort *v ), return; )

GL_FUNCTION(void, glRasterPos3dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glRasterPos3fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glRasterPos3iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glRasterPos3sv, ( const GLshort *v ), return; )

GL_FUNCTION(void, glRasterPos4dv, ( const GLdouble *v ), return; )
GL_FUNCTION(void, glRasterPos4fv, ( const GLfloat *v ), return; )
GL_FUNCTION(void, glRasterPos4iv, ( const GLint *v ), return; )
GL_FUNCTION(void, glRasterPos4sv, ( const GLshort *v ), return; )


GL_FUNCTION(void, glRectd, ( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 ), return; )
GL_FUNCTION(void, glRectf, ( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ), return; )
GL_FUNCTION(void, glRecti, ( GLint x1, GLint y1, GLint x2, GLint y2 ), return; )
GL_FUNCTION(void, glRects, ( GLshort x1, GLshort y1, GLshort x2, GLshort y2 ), return; )


GL_FUNCTION(void, glRectdv, ( const GLdouble *v1, const GLdouble *v2 ), return; )
GL_FUNCTION(void, glRectfv, ( const GLfloat *v1, const GLfloat *v2 ), return; )
GL_FUNCTION(void, glRectiv, ( const GLint *v1, const GLint *v2 ), return; )
GL_FUNCTION(void, glRectsv, ( const GLshort *v1, const GLshort *v2 ), return; )



/*
 * Vertex Arrays  (1.1)
 */

GL_FUNCTION(void, glVertexPointer, ( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ), return; )

GL_FUNCTION(void, glNormalPointer, ( GLenum type, GLsizei stride, const GLvoid *ptr ), return; )

GL_FUNCTION(void, glColorPointer, ( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ), return; )

GL_FUNCTION(void, glIndexPointer, ( GLenum type, GLsizei stride, const GLvoid *ptr ), return; )

GL_FUNCTION(void, glTexCoordPointer, ( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ), return; )

GL_FUNCTION(void, glEdgeFlagPointer, ( GLsizei stride, const GLvoid *ptr ), return; )

GL_FUNCTION(void, glGetPointerv, ( GLenum pname, void **params ), return; )

GL_FUNCTION(void, glArrayElement, ( GLint i ), return; )

GL_FUNCTION(void, glDrawArrays, ( GLenum mode, GLint first, GLsizei count ), return; )

GL_FUNCTION(void, glDrawElements, ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ), return; )

GL_FUNCTION(void, glInterleavedArrays, ( GLenum format, GLsizei stride, const GLvoid *pointer ), return; )


/*
 * Lighting
 */

GL_FUNCTION(void, glShadeModel, ( GLenum mode ), return; )

GL_FUNCTION(void, glLightf, ( GLenum light, GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glLighti, ( GLenum light, GLenum pname, GLint param ), return; )
GL_FUNCTION(void, glLightfv, ( GLenum light, GLenum pname, const GLfloat *params ), return; )
GL_FUNCTION(void, glLightiv, ( GLenum light, GLenum pname, const GLint *params ), return; )

GL_FUNCTION(void, glGetLightfv, ( GLenum light, GLenum pname, GLfloat *params ), return; )
GL_FUNCTION(void, glGetLightiv, ( GLenum light, GLenum pname, GLint *params ), return; )

GL_FUNCTION(void, glLightModelf, ( GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glLightModeli, ( GLenum pname, GLint param ), return; )
GL_FUNCTION(void, glLightModelfv, ( GLenum pname, const GLfloat *params ), return; )
GL_FUNCTION(void, glLightModeliv, ( GLenum pname, const GLint *params ), return; )

GL_FUNCTION(void, glMaterialf, ( GLenum face, GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glMateriali, ( GLenum face, GLenum pname, GLint param ), return; )
GL_FUNCTION(void, glMaterialfv, ( GLenum face, GLenum pname, const GLfloat *params ), return; )
GL_FUNCTION(void, glMaterialiv, ( GLenum face, GLenum pname, const GLint *params ), return; )

GL_FUNCTION(void, glGetMaterialfv, ( GLenum face, GLenum pname, GLfloat *params ), return; )
GL_FUNCTION(void, glGetMaterialiv, ( GLenum face, GLenum pname, GLint *params ), return; )

GL_FUNCTION(void, glColorMaterial, ( GLenum face, GLenum mode ), return; )




/*
 * Raster functions
 */

GL_FUNCTION(void, glPixelZoom, ( GLfloat xfactor, GLfloat yfactor ), return; )

GL_FUNCTION(void, glPixelStoref, ( GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glPixelStorei, ( GLenum pname, GLint param ), return; )

GL_FUNCTION(void, glPixelTransferf, ( GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glPixelTransferi, ( GLenum pname, GLint param ), return; )

GL_FUNCTION(void, glPixelMapfv, ( GLenum map, GLint mapsize, const GLfloat *values ), return; )
GL_FUNCTION(void, glPixelMapuiv, ( GLenum map, GLint mapsize, const GLuint *values ), return; )
GL_FUNCTION(void, glPixelMapusv, ( GLenum map, GLint mapsize, const GLushort *values ), return; )

GL_FUNCTION(void, glGetPixelMapfv, ( GLenum map, GLfloat *values ), return; )
GL_FUNCTION(void, glGetPixelMapuiv, ( GLenum map, GLuint *values ), return; )
GL_FUNCTION(void, glGetPixelMapusv, ( GLenum map, GLushort *values ), return; )

GL_FUNCTION(void, glBitmap, ( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap ), return; )

GL_FUNCTION(void, glReadPixels, ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ), return; )

GL_FUNCTION(void, glDrawPixels, ( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ), return; )

GL_FUNCTION(void, glCopyPixels, ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type ), return; )



/*
 * Stenciling
 */

GL_FUNCTION(void, glStencilFunc, ( GLenum func, GLint ref, GLuint mask ), return; )

GL_FUNCTION(void, glStencilMask, ( GLuint mask ), return; )

GL_FUNCTION(void, glStencilOp, ( GLenum fail, GLenum zfail, GLenum zpass ), return; )

GL_FUNCTION(void, glClearStencil, ( GLint s ), return; )



/*
 * Texture mapping
 */

GL_FUNCTION(void, glTexGend, ( GLenum coord, GLenum pname, GLdouble param ), return; )
GL_FUNCTION(void, glTexGenf, ( GLenum coord, GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glTexGeni, ( GLenum coord, GLenum pname, GLint param ), return; )

GL_FUNCTION(void, glTexGendv, ( GLenum coord, GLenum pname, const GLdouble *params ), return; )
GL_FUNCTION(void, glTexGenfv, ( GLenum coord, GLenum pname, const GLfloat *params ), return; )
GL_FUNCTION(void, glTexGeniv, ( GLenum coord, GLenum pname, const GLint *params ), return; )

GL_FUNCTION(void, glGetTexGendv, ( GLenum coord, GLenum pname, GLdouble *params ), return; )
GL_FUNCTION(void, glGetTexGenfv, ( GLenum coord, GLenum pname, GLfloat *params ), return; )
GL_FUNCTION(void, glGetTexGeniv, ( GLenum coord, GLenum pname, GLint *params ), return; )


GL_FUNCTION(void, glTexEnvf, ( GLenum target, GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glTexEnvi, ( GLenum target, GLenum pname, GLint param ), return; )

GL_FUNCTION(void, glTexEnvfv, ( GLenum target, GLenum pname, const GLfloat *params ), return; )
GL_FUNCTION(void, glTexEnviv, ( GLenum target, GLenum pname, const GLint *params ), return; )

GL_FUNCTION(void, glGetTexEnvfv, ( GLenum target, GLenum pname, GLfloat *params ), return; )
GL_FUNCTION(void, glGetTexEnviv, ( GLenum target, GLenum pname, GLint *params ), return; )


GL_FUNCTION(void, glTexParameterf, ( GLenum target, GLenum pname, GLfloat param ), return; )
GL_FUNCTION(void, glTexParameteri, ( GLenum target, GLenum pname, GLint param ), return; )

GL_FUNCTION(void, glTexParameterfv, ( GLenum target, GLenum pname, const GLfloat *params ), return; )
GL_FUNCTION(void, glTexParameteriv, ( GLenum target, GLenum pname, const GLint *params ), return; )

GL_FUNCTION(void, glGetTexParameterfv, ( GLenum target, GLenum pname, GLfloat *params), return; )
GL_FUNCTION(void, glGetTexParameteriv, ( GLenum target, GLenum pname, GLint *params ), return; )

GL_FUNCTION(void, glGetTexLevelParameterfv, ( GLenum target, GLint level, GLenum pname, GLfloat *params ), return; )
GL_FUNCTION(void, glGetTexLevelParameteriv, ( GLenum target, GLint level, GLenum pname, GLint *params ), return; )


GL_FUNCTION(void, glTexImage1D, ( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels ), return; )

GL_FUNCTION(void, glTexImage2D, ( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels ), return; )

GL_FUNCTION(void, glGetTexImage, ( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels ), return; )



/* 1.1 functions */

GL_FUNCTION(void, glGenTextures, ( GLsizei n, GLuint *textures ), return; )

GL_FUNCTION(void, glDeleteTextures, ( GLsizei n, const GLuint *textures), return; )

GL_FUNCTION(void, glBindTexture, ( GLenum target, GLuint texture ), return; )

GL_FUNCTION(void, glPrioritizeTextures, ( GLsizei n, const GLuint *textures, const GLclampf *priorities ), return; )

GL_FUNCTION(GLboolean, glAreTexturesResident, ( GLsizei n, const GLuint *textures, GLboolean *residences ), return GL_FALSE; )

GL_FUNCTION(GLboolean, glIsTexture, ( GLuint texture ), return GL_FALSE; )


GL_FUNCTION(void, glTexSubImage1D, ( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels ), return; )


GL_FUNCTION(void, glTexSubImage2D, ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ), return; )


GL_FUNCTION(void, glCopyTexImage1D, ( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border ), return; )


GL_FUNCTION(void, glCopyTexImage2D, ( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border ), return; )


GL_FUNCTION(void, glCopyTexSubImage1D, ( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width ), return; )


GL_FUNCTION(void, glCopyTexSubImage2D, ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height ), return; )




/*
 * Evaluators
 */

GL_FUNCTION(void, glMap1d, ( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points ), return; )
GL_FUNCTION(void, glMap1f, ( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points ), return; )

GL_FUNCTION(void, glMap2d, ( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points ), return; )
GL_FUNCTION(void, glMap2f, ( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points ), return; )

GL_FUNCTION(void, glGetMapdv, ( GLenum target, GLenum query, GLdouble *v ), return; )
GL_FUNCTION(void, glGetMapfv, ( GLenum target, GLenum query, GLfloat *v ), return; )
GL_FUNCTION(void, glGetMapiv, ( GLenum target, GLenum query, GLint *v ), return; )

GL_FUNCTION(void, glEvalCoord1d, ( GLdouble u ), return; )
GL_FUNCTION(void, glEvalCoord1f, ( GLfloat u ), return; )

GL_FUNCTION(void, glEvalCoord1dv, ( const GLdouble *u ), return; )
GL_FUNCTION(void, glEvalCoord1fv, ( const GLfloat *u ), return; )

GL_FUNCTION(void, glEvalCoord2d, ( GLdouble u, GLdouble v ), return; )
GL_FUNCTION(void, glEvalCoord2f, ( GLfloat u, GLfloat v ), return; )

GL_FUNCTION(void, glEvalCoord2dv, ( const GLdouble *u ), return; )
GL_FUNCTION(void, glEvalCoord2fv, ( const GLfloat *u ), return; )

GL_FUNCTION(void, glMapGrid1d, ( GLint un, GLdouble u1, GLdouble u2 ), return; )
GL_FUNCTION(void, glMapGrid1f, ( GLint un, GLfloat u1, GLfloat u2 ), return; )

GL_FUNCTION(void, glMapGrid2d, ( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 ), return; )
GL_FUNCTION(void, glMapGrid2f, ( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 ), return; )

GL_FUNCTION(void, glEvalPoint1, ( GLint i ), return; )

GL_FUNCTION(void, glEvalPoint2, ( GLint i, GLint j ), return; )

GL_FUNCTION(void, glEvalMesh1, ( GLenum mode, GLint i1, GLint i2 ), return; )

GL_FUNCTION(void, glEvalMesh2, ( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 ), return; )



/*
 * Fog
 */

GL_FUNCTION(void, glFogf, ( GLenum pname, GLfloat param ), return; )

GL_FUNCTION(void, glFogi, ( GLenum pname, GLint param ), return; )

GL_FUNCTION(void, glFogfv, ( GLenum pname, const GLfloat *params ), return; )

GL_FUNCTION(void, glFogiv, ( GLenum pname, const GLint *params ), return; )



/*
 * Selection and Feedback
 */

GL_FUNCTION(void, glFeedbackBuffer, ( GLsizei size, GLenum type, GLfloat *buffer ), return; )

GL_FUNCTION(void, glPassThrough, ( GLfloat token ), return; )

GL_FUNCTION(void, glSelectBuffer, ( GLsizei size, GLuint *buffer ), return; )

GL_FUNCTION(void, glInitNames, ( void ), return; )

GL_FUNCTION(void, glLoadName, ( GLuint name ), return; )

GL_FUNCTION(void, glPushName, ( GLuint name ), return; )

GL_FUNCTION(void, glPopName, ( void ), return; )



/* 1.2 functions */
GL_FUNCTION(void, glDrawRangeElements, ( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices ), return; )

GL_FUNCTION(void, glTexImage3D, ( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels ), return; )

GL_FUNCTION(void, glTexSubImage3D, ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels), return; )

GL_FUNCTION(void, glCopyTexSubImage3D, ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height ), return; )


/* 1.2 imaging extension functions */

GL_FUNCTION(void, glColorTable, ( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table ), return; )

GL_FUNCTION(void, glColorSubTable, ( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data ), return; )

GL_FUNCTION(void, glColorTableParameteriv, (GLenum target, GLenum pname, const GLint *params), return; )

GL_FUNCTION(void, glColorTableParameterfv, (GLenum target, GLenum pname, const GLfloat *params), return; )

GL_FUNCTION(void, glCopyColorSubTable, ( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width ), return; )

GL_FUNCTION(void, glCopyColorTable, ( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ), return; )

GL_FUNCTION(void, glGetColorTable, ( GLenum target, GLenum format, GLenum type, GLvoid *table ), return; )

GL_FUNCTION(void, glGetColorTableParameterfv, ( GLenum target, GLenum pname, GLfloat *params ), return; )

GL_FUNCTION(void, glGetColorTableParameteriv, ( GLenum target, GLenum pname, GLint *params ), return; )

GL_FUNCTION(void, glBlendEquation, ( GLenum mode ), return; )

GL_FUNCTION(void, glBlendColor, ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ), return; )

GL_FUNCTION(void, glHistogram, ( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink ), return; )

GL_FUNCTION(void, glResetHistogram, ( GLenum target ), return; )

GL_FUNCTION(void, glGetHistogram, ( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values ), return; )

GL_FUNCTION(void, glGetHistogramParameterfv, ( GLenum target, GLenum pname, GLfloat *params ), return; )

GL_FUNCTION(void, glGetHistogramParameteriv, ( GLenum target, GLenum pname, GLint *params ), return; )

GL_FUNCTION(void, glMinmax, ( GLenum target, GLenum internalformat, GLboolean sink ), return; )

GL_FUNCTION(void, glResetMinmax, ( GLenum target ), return; )

GL_FUNCTION(void, glGetMinmax, ( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values ), return; )

GL_FUNCTION(void, glGetMinmaxParameterfv, ( GLenum target, GLenum pname, GLfloat *params ), return; )

GL_FUNCTION(void, glGetMinmaxParameteriv, ( GLenum target, GLenum pname, GLint *params ), return; )

GL_FUNCTION(void, glConvolutionFilter1D, ( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image ), return; )

GL_FUNCTION(void, glConvolutionFilter2D, ( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image ), return; )

GL_FUNCTION(void, glConvolutionParameterf, ( GLenum target, GLenum pname, GLfloat params ), return; )

GL_FUNCTION(void, glConvolutionParameterfv, ( GLenum target, GLenum pname, const GLfloat *params ), return; )

GL_FUNCTION(void, glConvolutionParameteri, ( GLenum target, GLenum pname, GLint params ), return; )

GL_FUNCTION(void, glConvolutionParameteriv, ( GLenum target, GLenum pname, const GLint *params ), return; )

GL_FUNCTION(void, glCopyConvolutionFilter1D, ( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ), return; )

GL_FUNCTION(void, glCopyConvolutionFilter2D, ( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height), return; )

GL_FUNCTION(void, glGetConvolutionFilter, ( GLenum target, GLenum format, GLenum type, GLvoid *image ), return; )

GL_FUNCTION(void, glGetConvolutionParameterfv, ( GLenum target, GLenum pname, GLfloat *params ), return; )

GL_FUNCTION(void, glGetConvolutionParameteriv, ( GLenum target, GLenum pname, GLint *params ), return; )

GL_FUNCTION(void, glSeparableFilter2D, ( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column ), return; )
GL_FUNCTION(void, glGetSeparableFilter, ( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span ), return; )

GL_FUNCTION(void, glActiveTextureARB, (GLenum texture), return; )
GL_FUNCTION(void, glClientActiveTextureARB, (GLenum texture), return; )
GL_FUNCTION(void, glMultiTexCoord1dARB, (GLenum target, GLdouble s), return; )
GL_FUNCTION(void, glMultiTexCoord1dvARB, (GLenum target, const GLdouble *v), return; )
GL_FUNCTION(void, glMultiTexCoord1fARB, (GLenum target, GLfloat s), return; )
GL_FUNCTION(void, glMultiTexCoord1fvARB, (GLenum target, const GLfloat *v), return; )
GL_FUNCTION(void, glMultiTexCoord1iARB, (GLenum target, GLint s), return; )
GL_FUNCTION(void, glMultiTexCoord1ivARB, (GLenum target, const GLint *v), return; )
GL_FUNCTION(void, glMultiTexCoord1sARB, (GLenum target, GLshort s), return; )
GL_FUNCTION(void, glMultiTexCoord1svARB, (GLenum target, const GLshort *v), return; )
GL_FUNCTION(void, glMultiTexCoord2dARB, (GLenum target, GLdouble s, GLdouble t), return; )
GL_FUNCTION(void, glMultiTexCoord2dvARB, (GLenum target, const GLdouble *v), return; )
GL_FUNCTION(void, glMultiTexCoord2fARB, (GLenum target, GLfloat s, GLfloat t), return; )
GL_FUNCTION(void, glMultiTexCoord2fvARB, (GLenum target, const GLfloat *v), return; )
GL_FUNCTION(void, glMultiTexCoord2iARB, (GLenum target, GLint s, GLint t), return; )
GL_FUNCTION(void, glMultiTexCoord2ivARB, (GLenum target, const GLint *v), return; )
GL_FUNCTION(void, glMultiTexCoord2sARB, (GLenum target, GLshort s, GLshort t), return; )
GL_FUNCTION(void, glMultiTexCoord2svARB, (GLenum target, const GLshort *v), return; )
GL_FUNCTION(void, glMultiTexCoord3dARB, (GLenum target, GLdouble s, GLdouble t, GLdouble r), return; )
GL_FUNCTION(void, glMultiTexCoord3dvARB, (GLenum target, const GLdouble *v), return; )
GL_FUNCTION(void, glMultiTexCoord3fARB, (GLenum target, GLfloat s, GLfloat t, GLfloat r), return; )
GL_FUNCTION(void, glMultiTexCoord3fvARB, (GLenum target, const GLfloat *v), return; )
GL_FUNCTION(void, glMultiTexCoord3iARB, (GLenum target, GLint s, GLint t, GLint r), return; )
GL_FUNCTION(void, glMultiTexCoord3ivARB, (GLenum target, const GLint *v), return; )
GL_FUNCTION(void, glMultiTexCoord3sARB, (GLenum target, GLshort s, GLshort t, GLshort r), return; )
GL_FUNCTION(void, glMultiTexCoord3svARB, (GLenum target, const GLshort *v), return; )
GL_FUNCTION(void, glMultiTexCoord4dARB, (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q), return; )
GL_FUNCTION(void, glMultiTexCoord4dvARB, (GLenum target, const GLdouble *v), return; )
GL_FUNCTION(void, glMultiTexCoord4fARB, (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q), return; )
GL_FUNCTION(void, glMultiTexCoord4fvARB, (GLenum target, const GLfloat *v), return; )
GL_FUNCTION(void, glMultiTexCoord4iARB, (GLenum target, GLint s, GLint t, GLint r, GLint q), return; )
GL_FUNCTION(void, glMultiTexCoord4ivARB, (GLenum target, const GLint *v), return; )
GL_FUNCTION(void, glMultiTexCoord4sARB, (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q), return; )
GL_FUNCTION(void, glMultiTexCoord4svARB, (GLenum target, const GLshort *v), return; )

/* EXT_paletted_texture */
GL_FUNCTION(void, glColorTableEXT, (GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* data), return; )

/* EXT_compiled_vertex_array */
GL_FUNCTION(void, glLockArraysEXT, (GLint first, GLsizei count), return; )
GL_FUNCTION(void, glUnlockArraysEXT, (void), return; )

/* EXT_fog_coord */
GL_FUNCTION(void, glFogCoordfEXT, (GLfloat  coord), return; )
GL_FUNCTION(void, glFogCoordPointerEXT, (GLenum type, GLsizei stride, void *pointer), return; )

/* EXT_vertex_buffer */
GL_FUNCTION(GLboolean, glAvailableVertexBufferEXT, (void), return GL_FALSE; )
GL_FUNCTION(GLint, glAllocateVertexBufferEXT, (GLsizei size, GLint format, GLboolean preserve), return 0; )
GL_FUNCTION(void*, glLockVertexBufferEXT, (GLint handle, GLsizei size), return NULL; )
GL_FUNCTION(void, glUnlockVertexBufferEXT, (GLint handle), return; )
GL_FUNCTION(void, glSetVertexBufferEXT, (GLint handle), return; )
GL_FUNCTION(void, glOffsetVertexBufferEXT, (GLint handle, GLuint offset), return; )
GL_FUNCTION(void, glFillVertexBufferEXT, (GLint handle, GLint first, GLsizei count), return; )
GL_FUNCTION(void, glFreeVertexBufferEXT, (GLint handle), return; )
