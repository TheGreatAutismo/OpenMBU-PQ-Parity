//-----------------------------------------------------------------------------
// Collada-2-DTS
// Copyright (C) 2008 GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"

// Make GCC happy.  Needs to have seen this before processing the
// hash table template.
struct VertTuple;
namespace DictHash
{
   inline U32 hash( const VertTuple& data );
}

#include "ts/collada/colladaExtensions.h"
#include "ts/collada/colladaAppMesh.h"
#include "ts/collada/colladaAppNode.h"
#include "ts/collada/colladaAppMaterial.h"

#include "core/tDictionary.h"

using namespace ColladaUtils;

bool ColladaAppMesh::fixedSizeEnabled = false;
S32 ColladaAppMesh::fixedSize = 2;

//-----------------------------------------------------------------------------
// Define a VertTuple dictionary to allow fast tuple lookups
namespace DictHash
{
   inline U32 hash(const VertTuple& data)
   {
      return (U32)data.vertex;
   }
}

typedef Map<VertTuple, S32> VertTupleMap;

//-----------------------------------------------------------------------------
// Collada scatters the data required for geometry all over the place; this class
// helps to group it all together.
class MeshStreams
{
   /// Classify a single input
   template<class T>
   static void selectInput(T input, T sortedInputs[], S32 start, S32 end=-1)
   {
      if (end == -1)
         end = start;

      // Get the set for this input
      const domInputLocalOffset* localOffset = daeSafeCast<domInputLocalOffset>(input);
      domUint newSet = localOffset ? localOffset->getSet() : 0;

      // Add the input to the right place in the list (somewhere between start and end)
      for (S32 i = start; i <= end; i++) {
         const domInputLocalOffset* localOffset = daeSafeCast<domInputLocalOffset>(sortedInputs[i]);
         domUint set = localOffset ? localOffset->getSet() : 0xFFFFFFFF;
         if (newSet < set) {
            for (S32 j = i + 1; j <= end; j++)
               sortedInputs[j] = sortedInputs[j-1];
            sortedInputs[i] = input;
            return;
         }
      }
   }

   /// Attempt to initialise a _SourceReader
   template<class T>
   bool initSourceReader(T input, _SourceReader& reader, const char* params[])
   {
      if (!input)
         return false;

      // Try to get the source element
      const domSource* source = 0;
      daeElement *element = input->getSource().getElement();
      if (element->getElementType() == COLLADA_TYPE::SOURCE)
         source = daeSafeCast<domSource>(element);
      else if (element->getElementType() == COLLADA_TYPE::VERTICES) {
         const domVertices* vertices = daeSafeCast<domVertices>(element);
         if (!vertices || !vertices->getInput_array().getCount())
            return false;
         source = daeSafeCast<domSource>(vertices->getInput_array()[0]->getSource().getElement());
      }
      if (!source)
         return false;

      return reader.initFromSource(source, params);
   }

public:

   // The sources we want to read from the mesh stream. Can be any order, but
   // sources of the same type (eg. UVs and UV2s) must be sequential (to allow
   // ordering by set index)
   enum {
      Points,
      Normals,
      Colors,
      UVs,
      UV2s,
      Joints,
      Weights,
      InvBindMatrices,
      NumStreams
   };

   _SourceReader     points;
   _SourceReader     normals;
   _SourceReader     colors;
   _SourceReader     uvs;
   _SourceReader     uv2s;

   _SourceReader     joints;
   _SourceReader     weights;
   _SourceReader     invBindMatrices;

   /// Clear the mesh streams
   void reset()
   {
      points.reset();
      normals.reset();
      colors.reset();
      uvs.reset();
      uv2s.reset();
      joints.reset();
      weights.reset();
      invBindMatrices.reset();
   }

   /// Classify a set of inputs by type and set number (needs to be a template
   /// because Collada has two forms of input arrays that may be accessed in
   /// an identical fashion, but the classes are unrelated. Sigh.
   template<class T>
   static void classifyInputs(const daeTArray<T>& inputs, T sortedInputs[], U32 *maxOffset=0)
   {
      if (maxOffset)
         *maxOffset = 0;

      // Clear output array
      for (int i = 0; i < NumStreams; i++)
         sortedInputs[i] = 0;

      // Separate inputs by type, and sort by set (ie. lowest TEXCOORD set becomes UV,
      // next TEXCOORD set becomes UV2 etc)
      for (int iInput = 0; iInput < inputs.getCount(); iInput++) {

         const T& input = inputs[iInput];
         const daeString semantic = input->getSemantic();

         if (dStrEqual(semantic, "VERTEX"))                 selectInput(input, sortedInputs, Points);
         else if (dStrEqual(semantic, "NORMAL"))            selectInput(input, sortedInputs, Normals);
         else if (dStrEqual(semantic, "COLOR"))             selectInput(input, sortedInputs, Colors);
         else if (dStrEqual(semantic, "TEXCOORD"))          selectInput(input, sortedInputs, UVs, UV2s);
         else if (dStrEqual(semantic, "JOINT"))             selectInput(input, sortedInputs, Joints);
         else if (dStrEqual(semantic, "WEIGHT"))            selectInput(input, sortedInputs, Weights);
         else if (dStrEqual(semantic, "INV_BIND_MATRIX"))   selectInput(input, sortedInputs, InvBindMatrices);

         if (maxOffset)
         {
            const domInputLocalOffset* localOffset = daeSafeCast<domInputLocalOffset>(input);
            domUint offset = localOffset ? localOffset->getOffset() : 0;
            if (offset > (*maxOffset))
               *maxOffset = offset;
         }
      }
   }

   /// Read a set of inputs into the named sources. There may be multiple 'sets'
   /// of COLOR or TEXCOORD (uvs) streams, but we are only interested in the
   /// first COLOR set (ie. smallest set value), and the first 2 TEXCOORDS sets.
   template<class T>
   bool readInputs(const daeTArray<T>& inputs)
   {
      // Sort inputs by type and set to find the ones we are interested in
      T sortedInputs[NumStreams];
      classifyInputs(inputs, sortedInputs);

      // Attempt to initialise the SourceReaders
      const char* vertex_params[] = { "X", "Y", "Z", "" };
      initSourceReader(sortedInputs[Points], points, vertex_params);

      const char* normal_params[] = { "X", "Y", "Z", "" };
      initSourceReader(sortedInputs[Normals], normals, normal_params);

      const char* color_params[] = { "R", "G", "B", "A", "" };
      initSourceReader(sortedInputs[Colors], colors, color_params);

      const char* uv_params[] = { "S", "T", "" };
      const char* uv_params2[] = { "U", "V", "" }; // some files use the nonstandard U,V param names
      if (!initSourceReader(sortedInputs[UVs], uvs, uv_params))
         initSourceReader(sortedInputs[UVs], uvs, uv_params2);
      if (!initSourceReader(sortedInputs[UV2s], uv2s, uv_params))
         initSourceReader(sortedInputs[UV2s], uv2s, uv_params2);

      const char* joint_params[] = { "JOINT", "" };
      initSourceReader(sortedInputs[Joints], joints, joint_params);

      const char* weight_params[] = { "WEIGHT", "" };
      initSourceReader(sortedInputs[Weights], weights, weight_params);

      const char* matrix_params[] = { "TRANSFORM", "" };
      initSourceReader(sortedInputs[InvBindMatrices], invBindMatrices, matrix_params);

      return true;
   }
};

//------------------------------------------------------------------------------

ColladaAppMesh::ColladaAppMesh(const domInstance_geometry* instance, ColladaAppNode* node)
   : instanceGeom(instance), instanceCtrl(0), appNode(node), geomExt(0)
{
   flags = 0;
   numFrames = 0;
   numMatFrames = 0;
}

ColladaAppMesh::ColladaAppMesh(const domInstance_controller* instance, ColladaAppNode* node)
   : instanceGeom(0), instanceCtrl(instance), appNode(node), geomExt(0)
{
   flags = 0;
   numFrames = 0;
   numMatFrames = 0;
}

const char* ColladaAppMesh::getName(bool allowFixed)
{
   // Some exporters add a 'PIVOT' node between the mesh and the actual
   // object node. Detect this and return the object node name instead
   // of the pivot node.
   const char* nodeName = appNode->getName();
   if (dStrEndsWith(nodeName, "PIVOT"))
      nodeName = appNode->getParentName();

   // If all geometry is being fixed to the same size, append the size
   // to the name
   return allowFixed && fixedSizeEnabled ? avar("%s %d", nodeName, fixedSize) : nodeName;
}

MatrixF ColladaAppMesh::getMeshTransform(F32 time)
{
   return appNode->getNodeTransform(time);
}

bool ColladaAppMesh::animatesVis(const AppSequence* appSeq)
{
   #define IS_VIS_ANIMATED(node)    \
      (dynamic_cast<const ColladaAppNode*>(node)->nodeExt->visibility.isAnimated(appSeq->getStart(), appSeq->getEnd()))

   // Check if the node visibility is animated within the sequence interval
   return IS_VIS_ANIMATED(appNode) || (appNode->appParent ? IS_VIS_ANIMATED(appNode->appParent) : false);
}

bool ColladaAppMesh::animatesMatFrame(const AppSequence* appSeq)
{
   // Texture coordinates may be animated in two ways:
   // - by animating the MAYA profile texture transform (diffuse texture)
   // - by animating the morph weights for morph targets with different UVs

   // Check if the MAYA profile texture transform is animated
   for (int iMat = 0; iMat < appMaterials.size(); iMat++) {
      ColladaAppMaterial* appMat = static_cast<ColladaAppMaterial*>(appMaterials[iMat]);
      if (appMat->effectExt->animatesTextureTransform(appSeq->getStart(), appSeq->getEnd()))
         return true;
   }

   // Check that the morph weights are animated within the sequence interval, 
   // and that the morph targets have different UVs to the base geometry.
   bool animated = false;
   bool differentUVs = false;
   if (const domMorph* morph = getMorph()) {
      for (int iInput = 0; iInput < morph->getTargets()->getInput_array().getCount(); iInput++) {
         const domInputLocal* input = morph->getTargets()->getInput_array()[iInput];
         if (dStrEqual(input->getSemantic(), "MORPH_TARGET")) {
            // @todo: Check if morph targets have different UVs to base geometry
            differentUVs = false;
         }
         if (dStrEqual(input->getSemantic(), "MORPH_WEIGHT")) {
            const domSource* source = daeSafeCast<domSource>(input->getSource().getElement());
            AnimatedFloatList weights(source ? source->getFloat_array() : 0);
            animated = weights.isAnimated(appSeq->getStart(), appSeq->getEnd());
         }
      }
   }

   return (animated && differentUVs);
}

bool ColladaAppMesh::animatesFrame(const AppSequence* appSeq)
{
   // Collada <morph>s ALWAYS contain vert positions, so just need to check if
   // the morph weights are animated within the sequence interval
   bool animated = false;
   if (const domMorph* morph = getMorph()) {
      for (int iInput = 0; iInput < morph->getTargets()->getInput_array().getCount(); iInput++) {
         const domInputLocal* input = morph->getTargets()->getInput_array()[iInput];
         if (dStrEqual(input->getSemantic(), "MORPH_WEIGHT")) {
            const domSource* source = daeSafeCast<domSource>(input->getSource().getElement());
            AnimatedFloatList weights(source ? source->getFloat_array() : 0);
            animated = weights.isAnimated(appSeq->getStart(), appSeq->getEnd());
            break;
         }
      }
   }
   return animated;
}

F32 ColladaAppMesh::getVisValue(F32 t)
{
   #define GET_VIS(node)   \
      (dynamic_cast<const ColladaAppNode*>(node)->nodeExt->visibility.getValue(t))

   // Get the visibility of the mesh's node at time, 't'
   return GET_VIS(appNode) * (appNode->appParent ? GET_VIS(appNode->appParent) : 1.0f);
}

S32 ColladaAppMesh::addMaterial(const char* symbol)
{
   if (!symbol)
      return TSDrawPrimitive::NoMaterial;

   // Lookup the symbol in the materials already bound to this geometry/controller
   // instance
   Map<StringTableEntry,U32>::Iterator itr = boundMaterials.find(symbol);
   if (itr != boundMaterials.end())
      return itr->value;

   // Find the Collada material that this symbol maps to
   U32 matIndex = TSDrawPrimitive::NoMaterial;
   const domBind_material* binds = instanceGeom ? instanceGeom->getBind_material() :
                                                  instanceCtrl->getBind_material();
   if (binds) {
      const domInstance_material_Array& matArray = binds->getTechnique_common()->getInstance_material_array();
      for (int iBind = 0; iBind < matArray.getCount(); iBind++) {
         if (dStrEqual(matArray[iBind]->getSymbol(), symbol)) {

            // Find the index of the bound material in the shape global list
            const domMaterial* mat = daeSafeCast<domMaterial>(matArray[iBind]->getTarget().getElement());
            for (matIndex = 0; matIndex < appMaterials.size(); matIndex++) {
               if (static_cast<ColladaAppMaterial*>(appMaterials[matIndex])->mat == mat)
                  break;
            }

            // Check if this material needs to be added to the shape global list
            if (matIndex == appMaterials.size())
               appMaterials.push_back(new ColladaAppMaterial(mat));

            break;
         }
      }
   }

   // Add this symbol to the bound list for the mesh
   boundMaterials.insert(StringTable->insert(symbol), matIndex);
   return matIndex;
}

void ColladaAppMesh::getPrimitives(const domGeometry* geometry)
{
   // Only do this once
   if (primitives.size())
      return;

   // Read the <geometry> extension
   if (!geomExt)
      geomExt = new ColladaExtension_geometry(geometry);

   // Get the supported primitive elements for this geometry, and warn
   // about unsupported elements
   Vector<BasePrimitive*> meshPrims;
   const daeElementRefArray& contents = geometry->getMesh()->getContents();
   for (int iElem = 0; iElem < contents.getCount(); iElem++) {

      if (BasePrimitive::isPrimitive(contents[iElem])) {
         if (BasePrimitive::isSupportedPrimitive(contents[iElem]))
            meshPrims.push_back(BasePrimitive::get(contents[iElem]));
         else {
            daeErrorHandler::get()->handleWarning(avar("Collada <%s> element "
               "in %s is not supported.", contents[iElem]->getElementName(),
               _GetNameOrId(geometry)));
         }
      }
   }

   VertTupleMap tupleMap;

   // Create Torque primitives
   for (int iPrim = 0; iPrim < meshPrims.size(); iPrim++) {

      // Primitive element must have at least 1 triangle
      const domListOfUInts* pTriData = meshPrims[iPrim]->getTriangleData();
      if (!pTriData)
         continue;

      U32 numTriangles = pTriData->getCount() / meshPrims[iPrim]->getStride() / 3;
      if (!numTriangles)
         continue;

      // Create TSMesh primitive
      primitives.increment();
      TSDrawPrimitive& primitive = primitives.last();
      primitive.start = indices.size();
      primitive.matIndex = (TSDrawPrimitive::Triangles | TSDrawPrimitive::Indexed) |
                           addMaterial(meshPrims[iPrim]->getMaterial());

      // Get the AppMaterial associated with this primitive
      ColladaAppMaterial* appMat = 0;
      if (!(primitive.matIndex & TSDrawPrimitive::NoMaterial))
         appMat = static_cast<ColladaAppMaterial*>(appMaterials[primitive.matIndex & TSDrawPrimitive::MaterialMask]);

      // Force the material to be double-sided if this geometry is double-sided.
      if (geomExt->double_sided && appMat)
         appMat->effectExt->double_sided = true;

      // Pre-allocate triangle indices
      primitive.numElements = numTriangles * 3;
      indices.setSize(indices.size() + primitive.numElements);
      U16* dstIndex = indices.end() - primitive.numElements;

      // Determine the offset for each element type in the stream, and also the
      // maximum input offset, which will be the number of indices per vertex we
      // need to skip.
      domInputLocalOffsetRef sortedInputs[MeshStreams::NumStreams];
      MeshStreams::classifyInputs(meshPrims[iPrim]->getInputs(), sortedInputs);

      S32 offsets[MeshStreams::NumStreams];
      for (S32 i = 0; i < MeshStreams::NumStreams; i++)
         offsets[i] = sortedInputs[i] ? sortedInputs[i]->getOffset() : -1;

      // Loop through indices
      const domUint* pSrcData = &(pTriData->get(0));

      for (U32 iTri = 0; iTri < numTriangles; iTri++) {

         // If the next triangle could cause us to index across a 16-bit
         // boundary, split this primitive and clear the tuple map to
         // ensure primitives only index verts within a 16-bit range.
         if (vertTuples.size() &&
            (((vertTuples.size()-1) ^ (vertTuples.size()+2)) & 0x10000))
         {
            // Pad vertTuples up to the next 16-bit boundary
            while (vertTuples.size() & 0xFFFF)
               vertTuples.push_back(VertTuple(vertTuples.last()));

            // Split the primitive at the current triangle
            S32 indicesRemaining = (numTriangles - iTri) * 3;
            if (iTri > 0)
            {
               daeErrorHandler::get()->handleWarning(avar("Splitting primitive "
                  "in %s: too many verts for 16-bit indices.", _GetNameOrId(geometry)));

               primitives.last().numElements -= indicesRemaining;
               primitives.push_back(TSDrawPrimitive(primitives.last()));
            }

            primitives.last().numElements = indicesRemaining;
            primitives.last().start = indices.size() - indicesRemaining;

            tupleMap.clear();
         }

         for (U32 v = 0; v < 3; v++) {
            // Collect vert tuples into a single array so we can easily grab
            // vertex data later.
            VertTuple tuple;
            tuple.prim = iPrim;
            tuple.vertex = offsets[MeshStreams::Points]  >= 0 ? pSrcData[offsets[MeshStreams::Points]] : -1;
            tuple.normal = offsets[MeshStreams::Normals] >= 0 ? pSrcData[offsets[MeshStreams::Normals]] : -1;
            tuple.color  = offsets[MeshStreams::Colors]  >= 0 ? pSrcData[offsets[MeshStreams::Colors]] : -1;
            tuple.uv     = offsets[MeshStreams::UVs]     >= 0 ? pSrcData[offsets[MeshStreams::UVs]] : -1;
            tuple.uv2    = offsets[MeshStreams::UV2s]    >= 0 ? pSrcData[offsets[MeshStreams::UV2s]] : -1;

            VertTupleMap::Iterator itr = tupleMap.find(tuple);
            if (itr == tupleMap.end())
            {
               itr = tupleMap.insert(tuple, vertTuples.size());
               vertTuples.push_back(tuple);
            }

            // Collada uses CCW for front face and Torque uses the opposite, so
            // for normal (non-inverted) meshes, the indices are flipped.
            if (appNode->invertMeshes)
               dstIndex[v] = itr->value;
            else
               dstIndex[2 - v] = itr->value;

            pSrcData += meshPrims[iPrim]->getStride();
         }
         dstIndex += 3;
      }
   }

   for (int iPrim = 0; iPrim < meshPrims.size(); iPrim++)
      delete meshPrims[iPrim];
}

void ColladaAppMesh::getVertexData(const domGeometry* geometry, F32 time, const MatrixF& objectOffset,
                                   Vector<Point3F>& v_points, 
                                   Vector<Point3F>& v_norms, 
                                   Vector<ColorI>& v_colors,
                                   Vector<Point2F>& v_uvs,
                                   Vector<Point2F>& v_uv2s,
                                   bool appendValues)
{
   if (!primitives.size())
      return;

   MeshStreams streams;
   S32 lastPrimitive = -1;
   ColladaAppMaterial* appMat = 0;

   // Get the supported primitive elements for this geometry
   Vector<BasePrimitive*> meshPrims;
   const daeElementRefArray& contents = geometry->getMesh()->getContents();
   for (int iElem = 0; iElem < contents.getCount(); iElem++) {
      if (BasePrimitive::isSupportedPrimitive(contents[iElem]))
         meshPrims.push_back(BasePrimitive::get(contents[iElem]));
   }

   // If appending values, pre-allocate the arrays
   if (appendValues) {
      v_points.setSize(v_points.size() + vertTuples.size());
      v_norms.setSize(v_norms.size() + vertTuples.size());
      v_uvs.setSize(v_uvs.size() + vertTuples.size());
   }

   // Get pointers to arrays
   Point3F* points_array = &v_points[v_points.size() - vertTuples.size()];
   Point3F* norms_array = &v_norms[v_norms.size() - vertTuples.size()];
   Point2F* uvs_array = &v_uvs[v_uvs.size() - vertTuples.size()];
   ColorI*  colors_array = NULL;
   Point2F* uv2s_array = NULL;

   for (int iVert = 0; iVert < vertTuples.size(); iVert++) {

      const VertTuple& tuple = vertTuples[iVert];

      // Change primitives?
      if (tuple.prim != lastPrimitive) {
         if (meshPrims.size() <= tuple.prim) {
            daeErrorHandler::get()->handleError(avar("Failed to get vertex data "
               "for %s. Primitives do not match base geometry.", geometry->getID()));
            break;
         }

         // Update vertex/normal/UV streams and get the new material index
         streams.reset();
         streams.readInputs(meshPrims[tuple.prim]->getInputs());
         S32 matIndex = addMaterial(meshPrims[tuple.prim]->getMaterial());
         if (matIndex != TSDrawPrimitive::NoMaterial)
            appMat = static_cast<ColladaAppMaterial*>(appMaterials[matIndex]);
         else
            appMat = 0;

         lastPrimitive = tuple.prim;
      }

      // If we are NOT appending values, only set the value if it actually exists
      // in the mesh data stream.

      if (appendValues || ((tuple.vertex >= 0) && (tuple.vertex < streams.points.size()))) {
         points_array[iVert] = streams.points.getPoint3FValue(tuple.vertex);

         // Flip verts for inverted meshes
         if (appNode->invertMeshes)
            points_array[iVert].z = -points_array[iVert].z;

         objectOffset.mulP(points_array[iVert]);
      }

      if (appendValues || ((tuple.normal >= 0) && (tuple.normal < streams.normals.size()))) {
         norms_array[iVert] = streams.normals.getPoint3FValue(tuple.normal);

         // Flip normals for inverted meshes
         if (appNode->invertMeshes)
            norms_array[iVert].z = -norms_array[iVert].z;
      }

      if (appendValues || ((tuple.uv >= 0) && (tuple.uv < streams.uvs.size()))) {
         uvs_array[iVert] = streams.uvs.getPoint2FValue(tuple.uv);
         if (appMat)
            appMat->effectExt->applyTextureTransform(uvs_array[iVert], time);
         uvs_array[iVert].y = 1.0f - uvs_array[iVert].y;   // Collada texcoords are upside down compared to TGE
      }

      // The rest is non-required data... if it doesn't exist then don't append it.

      if ( (tuple.color >= 0) && (tuple.color < streams.colors.size())) 
      {
         if ( !colors_array && iVert == 0 )
         {
            v_colors.setSize(v_colors.size() + vertTuples.size());
            colors_array = &v_colors[v_colors.size() - vertTuples.size()];
         }

         if ( colors_array )
            colors_array[iVert] = streams.colors.getColorIValue(tuple.color);
      }

      if ( (tuple.uv2 >= 0) && (tuple.uv2 < streams.uv2s.size()) ) 
      {
         if ( !uv2s_array && iVert == 0 )
         {
            v_uv2s.setSize(v_uv2s.size() + vertTuples.size());
            uv2s_array = &v_uv2s[v_uv2s.size() - vertTuples.size()];
         }

         if ( uv2s_array )
         {
            uv2s_array[iVert] = streams.uv2s.getPoint2FValue(tuple.uv2);
            if (appMat)
               appMat->effectExt->applyTextureTransform(uv2s_array[iVert], time);
            uv2s_array[iVert].y = 1.0f - uv2s_array[iVert].y;   // Collada texcoords are upside down compared to TGE
         }
      }
   }

   for (int iPrim = 0; iPrim < meshPrims.size(); iPrim++)
      delete meshPrims[iPrim];
}

void ColladaAppMesh::getMorphVertexData(const domMorph* morph, F32 time, const MatrixF& objectOffset,
                                        Vector<Point3F>& v_points, 
                                        Vector<Point3F>& v_norms, 
                                        Vector<ColorI>& v_colors, 
                                        Vector<Point2F>& v_uvs,
                                        Vector<Point2F>& v_uv2s)
{
   // @todo: Could the base geometry (or any target geometry) also be a morph?

   // Get the target geometries and weights (could be animated)
   Vector<const domGeometry*> targetGeoms;
   domListOfFloats targetWeights;

   for (int iInput = 0; iInput < morph->getTargets()->getInput_array().getCount(); iInput++) {
      const domInputLocal* input = morph->getTargets()->getInput_array()[iInput];
      const domSource* source = daeSafeCast<domSource>(input->getSource().getElement());

      if (dStrEqual(input->getSemantic(), "MORPH_TARGET")) {
         //  Get the morph targets
         _SourceReader srcTargets;
         srcTargets.initFromSource(source);

         for (int iTarget = 0; iTarget < srcTargets.size(); iTarget++) {
            // Lookup the element and add to the targets list
            daeIDRef idref(srcTargets.getStringValue(iTarget));
            idref.setContainer(morph->getDocument()->getDomRoot());
            targetGeoms.push_back(daeSafeCast<domGeometry>(idref.getElement()));
         }
      }
      else if (dStrEqual(input->getSemantic(), "MORPH_WEIGHT")) {
         //  Get the (possibly animated) morph weight
         targetWeights = AnimatedFloatList(source->getFloat_array()).getValue(time);
      }
   }

   // Check that we have a weight for each target
   if (targetGeoms.size() != targetWeights.getCount())
   {
      domController* ctrl = daeSafeCast<domController>(const_cast<domMorph*>(morph)->getParent());
      Con::warnf("Mismatched morph targets and weights in %s.", _GetNameOrId(ctrl));

      // Set unused targets to zero weighting (unused weights are ignored)
      while (targetGeoms.size() > targetWeights.getCount())
         targetWeights.append(0.0f);
   }

   // Get the base geometry and vertex data
   const domGeometry* baseGeometry = daeSafeCast<domGeometry>(morph->getSource().getElement());
   if (!baseGeometry)
      return;

   getPrimitives(baseGeometry);
   getVertexData(baseGeometry, time, objectOffset, v_points, v_norms, v_colors, v_uvs, v_uv2s, true);

   // Get pointers to the arrays of base geometry data
   Point3F* points_array = &v_points[v_points.size() - vertTuples.size()];
   Point3F* norms_array = &v_norms[v_norms.size() - vertTuples.size()];
   Point2F* uvs_array = &v_uvs[v_uvs.size() - vertTuples.size()];
   ColorI* colors_array = v_colors.size() ? &v_colors[v_colors.size() - vertTuples.size()] : 0;
   Point2F* uv2s_array = v_uv2s.size() ? &v_uv2s[v_uv2s.size() - vertTuples.size()] : 0;

   // Normalize base vertex data?
   if (morph->getMethod() == MORPHMETHODTYPE_NORMALIZED) {

      F32 weightSum = 0.0f;
      for (int iWeight = 0; iWeight < targetWeights.getCount(); iWeight++) {
         weightSum += targetWeights[iWeight];
      }

      // Result = Base*(1.0-w1-w2 ... -wN) + w1*Target1 + w2*Target2 ... + wN*TargetN
      weightSum = mClampF(1.0f - weightSum, 0.0f, 1.0f);

      for (int iVert = 0; iVert < vertTuples.size(); iVert++) {
         points_array[iVert] *= weightSum;
         norms_array[iVert] *= weightSum;
         uvs_array[iVert] *= weightSum;
      }

      if (uv2s_array) {
         for (int iVert = 0; iVert < vertTuples.size(); iVert++)
            uv2s_array[iVert] *= weightSum;
      }
   }

   // Interpolate using the target geometry and weights
   for (int iTarget = 0; iTarget < targetGeoms.size(); iTarget++) {

      // Ignore empty weights
      if (targetWeights[iTarget] == 0.0f)
         continue;

      // Get target geometry data into temporary arrays
      Vector<Point3F> targetPoints;
      Vector<Point3F> targetNorms;
      Vector<Point2F> targetUvs;
      Vector<ColorI>  targetColors;
      Vector<Point2F> targetUv2s;

      // Copy base geometry into target geometry (will be used if target does
      // not define normals or uvs)
      targetPoints.set(points_array, vertTuples.size());
      targetNorms.set(norms_array, vertTuples.size());
      targetUvs.set(uvs_array, vertTuples.size());
      if (colors_array)
         targetColors.set(colors_array, vertTuples.size());
      if (uv2s_array)
         targetUv2s.set(uv2s_array, vertTuples.size());

      getVertexData(targetGeoms[iTarget], time, objectOffset, targetPoints, targetNorms, targetColors, targetUvs, targetUv2s, false);

      // Combine with base geometry
      for (int iVert = 0; iVert < vertTuples.size(); iVert++) {
         points_array[iVert] += targetPoints[iVert] * targetWeights[iTarget];
         norms_array[iVert] += targetNorms[iVert] * targetWeights[iTarget];
         uvs_array[iVert] += targetUvs[iVert] * targetWeights[iTarget];
      }

      if (uv2s_array) {
         for (int iVert = 0; iVert < vertTuples.size(); iVert++)
            uv2s_array[iVert] += targetUv2s[iVert] * targetWeights[iTarget];
      }
      if (colors_array) {
         for (int iVert = 0; iVert < vertTuples.size(); iVert++)
            colors_array[iVert] += targetColors[iVert] * (F32)targetWeights[iTarget];
      }
   }
}

void ColladaAppMesh::lockMesh(F32 t, const MatrixF& objectOffset)
{
   // Find the geometry element for this mesh. Could be one of 3 things:
   // 1) a simple static mesh (Collada <geometry> element)
   // 2) a simple morph (some combination of static meshes)
   // 3) a skin (skin geometry could also be a morph!)
   daeElement* geometry = 0;

   if (instanceGeom) {
      // Simple, static mesh
      geometry = instanceGeom->getUrl().getElement();
   }
   else if (instanceCtrl) {
      const domController* ctrl = daeSafeCast<domController>(instanceCtrl->getUrl().getElement());
      if (!ctrl) {
         daeErrorHandler::get()->handleWarning(avar("Failed to find <controller> "
            "element for %s", getName()));
         return;
      }
      else if (ctrl->getMorph()) {
         // Morph controller
         geometry = ctrl->getMorph();
      }
      else {
         // Skinned mesh: source geometry could be static geometry or a morph controller.
         geometry = ctrl->getSkin()->getSource().getElement();
         if (geometry->getElementType() == COLLADA_TYPE::CONTROLLER)
            geometry = daeSafeCast<domController>(geometry)->getMorph();
      }
   }

   if (!geometry) {
      daeErrorHandler::get()->handleWarning(avar("Failed to find source geometry "
         "for %s", getName()));
      return;
   }

   // Now get the vertex data at the specified time
   if (geometry->getElementType() == COLLADA_TYPE::GEOMETRY) {
      getPrimitives(daeSafeCast<domGeometry>(geometry));
      getVertexData(daeSafeCast<domGeometry>(geometry), t, objectOffset, points, normals, colors, uvs, uv2s, true);
   }
   else if (geometry->getElementType() == COLLADA_TYPE::MORPH) {
      getMorphVertexData(daeSafeCast<domMorph>(geometry), t, objectOffset, points, normals, colors, uvs, uv2s);
   }
   else {
      daeErrorHandler::get()->handleWarning(avar("Unsupported geometry type "
         "'<%s>' for %s", geometry->getElementName(), getName()));
   }
}

void ColladaAppMesh::lookupSkinData()
{
   // Only lookup skin data once
   if (!isSkin() || weight.size())
      return;

   // Get the skin and vertex weight data
   const domSkin* skin = daeSafeCast<domController>(instanceCtrl->getUrl().getElement())->getSkin();
   const domSkin::domVertex_weights& weightIndices = *(skin->getVertex_weights());
   const domListOfInts& weights_v = weightIndices.getV()->getValue();
   const domListOfUInts& weights_vcount = weightIndices.getVcount()->getValue();

   MeshStreams streams;
   streams.readInputs(skin->getJoints()->getInput_array());
   streams.readInputs(weightIndices.getInput_array());

   MatrixF invObjOffset(objectOffset);
   invObjOffset.inverse();

   // Get the bind shape matrix
   MatrixF bindShapeMatrix(true);
   if (skin->getBind_shape_matrix())
      bindShapeMatrix = vecToMatrixF<domMatrix>(skin->getBind_shape_matrix()->getValue());
   bindShapeMatrix.mul(invObjOffset);

   // Determine the offset into the vindices array for each vertex (since each
   // vertex may have multiple [bone, weight] pairs in the array)
   Vector<U32> vindicesOffset;
   const domInt* vindices = (domInt*)weights_v.getRaw(0);
   for (int iWeight = 0; iWeight < weights_vcount.getCount(); iWeight++) {
      // Store the offset into the vindices array for this vertex
      vindicesOffset.push_back(vindices - (domInt*)weights_v.getRaw(0));
      vindices += (weights_vcount[iWeight]*2); // 2 indices [bone, weight] per vert
   }

   // Set vertex weights
   bool tooManyWeightsWarning = false;
   for (int iVert = 0; iVert < vertsPerFrame; iVert++) {
      const domUint* vcount = (domUint*)weights_vcount.getRaw(0);
      const domInt* vindices = (domInt*)weights_v.getRaw(0);
      vindices += vindicesOffset[vertTuples[iVert].vertex];

      // Limit the number of weights per bone (keep the N largest influences)
      if (vcount[vertTuples[iVert].vertex] > 16)
      {
         if (!tooManyWeightsWarning)
         {
            tooManyWeightsWarning = true;
            daeErrorHandler::get()->handleWarning(avar("At least one vertex has "
               "too many bone weights. Limiting to the largest %d influences.",
               16));
         }
      }

      for (int iWeight = 0; iWeight < vcount[vertTuples[iVert].vertex]; iWeight++) {
         if (iWeight >= 16)
         {
            // Too many weights => find and replace the smallest one
            S32 minIndex = weight.size() - 16;
            F32 minWeight = weight[minIndex];
            for (S32 i = minIndex + 1; i < weight.size(); i++)
            {
               if (weight[i] < minWeight)
               {
                  minWeight = weight[i];
                  minIndex = i;
               }
            }

            boneIndex[minIndex] = vindices[iWeight*2];
            weight[minIndex] = streams.weights.getFloatValue(vindices[iWeight*2 + 1]);
         }
         else
         {
            vertexIndex.push_back(iVert);
            boneIndex.push_back(vindices[iWeight*2]);
            weight.push_back(streams.weights.getFloatValue(vindices[iWeight*2 + 1]));
         }
      }
   }

   // Normalize vertex weights (force weights for each vert to sum to 1)
   int iWeight = 0;
   while (iWeight < weight.size()) {
      // Find the last weight with the same vertex number, and sum all weights for
      // that vertex
      F32 invTotalWeight = 0;
      int iLast;
      for (iLast = iWeight; iLast < weight.size(); iLast++) {
         if (vertexIndex[iLast] != vertexIndex[iWeight])
            break;
         invTotalWeight += weight[iLast];
      }

      // Then normalize the vertex weights
      invTotalWeight = 1.0f / invTotalWeight;
      for (; iWeight < iLast; iWeight++)
         weight[iWeight] *= invTotalWeight;
   }

   // Add dummy AppNodes to allow Collada joints to be mapped to 3space nodes
   bones.setSize(streams.joints.size());
   initialTransforms.setSize(streams.joints.size());
   for (int iJoint = 0; iJoint < streams.joints.size(); iJoint++)
   {
      const char* jointName = streams.joints.getStringValue(iJoint);

      // Lookup the joint element
      const domNode* joint = 0;
      if (instanceCtrl->getSkeleton_array().getCount()) {
         // Search for the node using the <skeleton> as the base element
         for (int iSkel = 0; iSkel < instanceCtrl->getSkeleton_array().getCount(); iSkel++) {
            xsAnyURI skeleton = instanceCtrl->getSkeleton_array()[iSkel]->getValue();
            daeSIDResolver resolver(skeleton.getElement(), jointName);
            joint = daeSafeCast<domNode>(resolver.getElement());
            if (joint)
               break;
         }
      }
      else {
         // Search for the node from the root level
         daeSIDResolver resolver(skin->getDocument()->getDomRoot(), jointName);
         joint = daeSafeCast<domNode>(resolver.getElement());
      }

      if (!joint) {
         daeErrorHandler::get()->handleWarning(avar("Failed to find bone '%s', "
            "defaulting to instance_controller parent node '%s'", jointName, appNode->getName()));
         joint = appNode->getDomNode();
      }
      bones[iJoint] = new ColladaAppNode(joint);

      initialTransforms[iJoint] = objectOffset;

      // Bone scaling is generally ignored during import, since 3space only
      // stores default node transform and rotation. Compensate for this by
      // removing the scaling from the inverse bind transform as well
      MatrixF invBind = streams.invBindMatrices.getMatrixFValue(iJoint);
      if (!ColladaUtils::getOptions().ignoreNodeScale)
      {
         Point3F invScale = invBind.getScale();
         invScale.x = invScale.x ? (1.0f / invScale.x) : 0;
         invScale.y = invScale.y ? (1.0f / invScale.y) : 0;
         invScale.z = invScale.z ? (1.0f / invScale.z) : 0;
         initialTransforms[iJoint].scale(invScale);
      }

      // Inverted node coordinate spaces (negative scale factor) are corrected
      // in ColladaAppNode::getNodeTransform, so need to apply the same operation
      // here to match
      if (m_matF_determinant(invBind) < 0.0f)
         initialTransforms[iJoint].scale(Point3F(1, 1, -1));

      initialTransforms[iJoint].mul(invBind);
      initialTransforms[iJoint].mul(bindShapeMatrix);
   }
}
