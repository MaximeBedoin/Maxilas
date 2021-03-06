package signswig;
/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.33
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class Obs {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected Obs(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Obs obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      signJNI.delete_Obs(swigCPtr);
    }
    swigCPtr = 0;
  }

  public boolean changeCoordinates(SWIGTYPE_p_p_double v, int lrow, int urow, int nc) {
    return signJNI.Obs_changeCoordinates(swigCPtr, this, SWIGTYPE_p_p_double.getCPtr(v), lrow, urow, nc);
  }

  public void reduceParameters(SWIGTYPE_p_p_double mean, SWIGTYPE_p_p_double std) {
    signJNI.Obs_reduceParameters__SWIG_0(swigCPtr, this, SWIGTYPE_p_p_double.getCPtr(mean), SWIGTYPE_p_p_double.getCPtr(std));
  }

  public void reduceParameters() {
    signJNI.Obs_reduceParameters__SWIG_1(swigCPtr, this);
  }

  public void selectParameters(SWIGTYPE_p_int index, int Num) {
    signJNI.Obs_selectParameters(swigCPtr, this, SWIGTYPE_p_int.getCPtr(index), Num);
  }

  public void selectParametersSwig(int num) {
    signJNI.Obs_selectParametersSwig(swigCPtr, this, num);
  }

  public void normalizeParameters(SWIGTYPE_p_double mean, SWIGTYPE_p_double std) {
    signJNI.Obs_normalizeParameters__SWIG_0(swigCPtr, this, SWIGTYPE_p_double.getCPtr(mean), SWIGTYPE_p_double.getCPtr(std));
  }

  public void normalizeParameters(SWIGTYPE_p_double normFactor) {
    signJNI.Obs_normalizeParameters__SWIG_1(swigCPtr, this, SWIGTYPE_p_double.getCPtr(normFactor));
  }

  public boolean save(String filename, boolean createNew) {
    return signJNI.Obs_save(swigCPtr, this, filename, createNew);
  }

  public void transpose() {
    signJNI.Obs_transpose(swigCPtr, this);
  }

  public boolean addObs(Obs obs_supl, int index, int size) {
    return signJNI.Obs_addObs(swigCPtr, this, Obs.getCPtr(obs_supl), obs_supl, index, size);
  }

  public int getObsSize() {
    return signJNI.Obs_getObsSize(swigCPtr, this);
  }

  public int getVecSize() {
    return signJNI.Obs_getVecSize(swigCPtr, this);
  }

  public void dump() {
    signJNI.Obs_dump(swigCPtr, this);
  }

  public boolean isInit() {
    return signJNI.Obs_isInit(swigCPtr, this);
  }

  public Obs(int L, int T) {
    this(signJNI.new_Obs__SWIG_0(L, T), true);
  }

  public Obs(String filename) {
    this(signJNI.new_Obs__SWIG_1(filename), true);
  }

  public Obs(String filename, int L_) {
    this(signJNI.new_Obs__SWIG_2(filename, L_), true);
  }

  public Obs(String filename, int L_, int T_) {
    this(signJNI.new_Obs__SWIG_3(filename, L_, T_), true);
  }

  public Obs getme() {
    long cPtr = signJNI.Obs_getme(swigCPtr, this);
    return (cPtr == 0) ? null : new Obs(cPtr, false);
  }

}
