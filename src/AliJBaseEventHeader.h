////////////////////////////////////////////////////
/*!
  \file AliJBaseEventHeader.h
  \brief
  \author O. Saarimaki, D.J.Kim (University of Jyvaskyla), Changwook Park (McGill University)
  \email: djkim@jyu.fi
  \version $Revision: 1.6 $
  \date $Date: 2018/08/22 15:36:00 $
  */
////////////////////////////////////////////////////

#ifndef ALIJBASEEVENTHEADER_H
#define ALIJBASEEVENTHEADER_H

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <TNamed.h>

class AliJBaseEventHeader : public TNamed {

 public:
  AliJBaseEventHeader();                                    // default constructor
  AliJBaseEventHeader(int eventid, float cent, float vtxz); // constructor
  AliJBaseEventHeader(const AliJBaseEventHeader& a);                    // copy constructor
  virtual ~AliJBaseEventHeader(){;}                         // destructor

  //getter
  int    GetEventID() const {return fEventID;} 
  float  GetCentrality() const {return fCentrality;}
  float  GetXVertex() const {return fVtxX;}
  float  GetYVertex() const {return fVtxY;}
  float  GetZVertex() const {return fVtxZ;}
  float  GetZertexErr() const {return fVtxZErr;}

  float  GetXVertexMC() const {return fVtxMCX;} 
  float  GetYVertexMC() const {return fVtxMCY;} 
  float  GetZVertexMC() const {return fVtxMCZ;} 

  float GetSigmaNN() const {return fSigmaNN;}
  float GetSigmaGen() const {return fSigmaGen;}

  //setter
  void SetEventID(int evid) {fEventID=evid;}
  void SetCentrality(float  cent) {fCentrality=cent;}
  void SetXVertex(float vt) {fVtxX=vt;}
  void SetYVertex(float vt) {fVtxY=vt;}
  void SetZVertex(float vt) {fVtxZ=vt;}
  void SetZVertexErr(float vt) {fVtxZErr=vt;}
  void SetVertex(float x, float y, float z, float err){ fVtxX=x;fVtxY=y;fVtxZ=z;fVtxZErr=err; }

  void SetXVertexMC(float vt) {fVtxMCX=vt;} 
  void SetYVertexMC(float vt) {fVtxMCY=vt;} 
  void SetZVertexMC(float vt) {fVtxMCZ=vt;}

  void SetSigmaNN(float sgm) {fSigmaNN=sgm;}
  void SetSigmaGen(float sgm) {fSigmaGen=sgm;}

  AliJBaseEventHeader& operator=(const AliJBaseEventHeader& header);

 private:

  Int_t   fEventID;         //event id
  Double32_t fCentrality;   //centrality
  Double32_t fVtxX;         //vertex X
  Double32_t fVtxY;         //vertex Y
  Double32_t fVtxZ;         //vertex Z
  Double32_t fVtxZErr;      //vertex error

  Double32_t fVtxMCX; //vertex X MC  
  Double32_t fVtxMCY; //vertex Y MC 
  Double32_t fVtxMCZ; //vertex Z MC

  Double32_t fSigmaNN;  //inelastic cross section
  Double32_t fSigmaGen; //jet cross section

  ClassDef(AliJBaseEventHeader,1)

};

#endif
