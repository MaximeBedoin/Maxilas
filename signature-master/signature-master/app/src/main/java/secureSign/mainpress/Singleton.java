package secureSign.mainpress;

import java.io.File;

import android.app.Activity;
import android.content.Context;

// Java Singleton Singleton.getInstance()
public final class Singleton {
  private static volatile Singleton instance = null;


  	public int			captureReason;	
	public int			trainBaseIndex;
	/************** Signature information  ***********************/

	/****************** HMM configuration  ***********************/
	public int SIGNMAX;
	public int STATE;
	public int GAUSSIAN;
	public double THRESHOLD;
	/*************************************************************/
  
	/************ HMM define *************************************/
	/*The maximum number of iteration to train the HMM*/
	static final int MAX_ITER_HMM = 30;

	/*end accuracy, stop criteria, training precision*/
	static final double END_ACCURACY = 1e-2;

	/*if 0, use END_ACCURACY as stop criteria
	if not, stop the training process after END_ITERATION_NUMBER*/
	static final int END_ITERATION_NUMBER = 0;	


	/*Definition of some HMM's topologies
	In this librery, we will install only the Left-Right and Full topologies */
	static final int TRANS_LR = 1;	//left-right
	static final int TRANS_BAKIS = 2;
	static final int TRANS_ERGODIC = 3;	//full connexion
	
	/****************** GMM define *******************************/
	static final double LOG_ZERO = -1e5;	//ou MIN_DOUBLE

	// Take attention to this value: the minimum probability
	// In our program, we compute the logarithme of the probability,
	// then we have to prevent some very low value of probability.

	// To obtain good result when using the GMM_SHORT, set it to 1e-100
	// when using the float HMM or GMM, pls set it to 1e-40: seat-of-the-pants experience
	static final double PRO_MIN_VAL = 1e-40;

	static final int COV_DIAG = 1;	
	static final int COV_FULL = 2;

	//The default standard deviation when using the parameter normalization option
	static final int PARAMETER_STANDARD_VARIATION = 2;	

	static final boolean PARAMETER_NORMALIZATION_YES = true;	
	static final boolean PARAMETER_NORMALIZATION_NO = false;
	
	static final double MAX_DOUBLE = 1e300;
	static final int X_STANDARD_VAR = 100;		//pour normaliser la taille des signatures
	static final double PI =	3.14159265358979323846;
	static final double MIN_DOUBLE = -1e300;
	
	public String actualName;
	public String actualCode;
	public identities iden;
  /**
   * Constructor
   */
  private Singleton() {
      // La pr�sence d'un constructeur priv� supprime
      // le constructeur public par d�faut.
      // Qui plus est seul le singleton peut s'instancier lui m�me
      super();
  }

  /**
   * Donne l'instance courante du singleton.
   * @return Retourne l'instance du singleton.
   */
  public final static Singleton getInstance() {
      //Le "Double-Checked Singleton"/"Singleton doublement v�rifi�" permet d'�viter un appel co�teux � synchronized, 
      //une fois que l'instanciation est faite.
      if (Singleton.instance == null) {
         // Le mot-cl� synchronized sur ce bloc emp�che 
         // toute instanciation multiple m�me par diff�rents threads.
         // Il est TRES important.
         synchronized(Singleton.class) {
           if (Singleton.instance == null) {
             Singleton.instance = new Singleton();
           }
         }
      }
      return Singleton.instance;
  }

	/**
	**  FUNCTION: GetModuleFilePath()
	**  
	**  USAGE : Deprecated !
	**  Better use directly activity.getBaseContext().getDir("main", Context.MODE_WORLD_READABLE).getAbsolutePath()
	*/  
  void GetModuleFilePath(StringBuffer pFilePath, int nSize, Activity activity)
	{

  	//Get program file path
  	String lpFileName = new String();
  	lpFileName = activity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
  	int len = lpFileName.length();
  	
  	int i;
  	for(i = len - 1; i > 0; i--){
  		if(lpFileName.charAt(i) == '/')
  			break;
  	}
  	
  	pFilePath.append(lpFileName.substring(0, i + 1));
	}
  
  /**
  **  FUNCTION: CreateDirIfNeeded()
  **  
  **  PARAM : the Directory Path
  **
  **  USAGE : mkdir if needed
  **  Deprecated : mkdirs alone do the same...
  **
  */  
  void CreateDirIfNeeded(String filename)
  {
	  	int len = filename.length();
	  	
	  	int i;
	  	for(i = len - 1; i > 0; i--){
	  		if(filename.charAt(i) == '/')
	  			break;
	  	}
	  	
	  	String lpFileName = new String(filename.substring(0, i + 1));	  
	  	File f = new File(lpFileName);
	  	f.mkdirs();
  }
}