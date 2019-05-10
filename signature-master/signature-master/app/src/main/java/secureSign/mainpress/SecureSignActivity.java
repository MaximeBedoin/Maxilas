package secureSign.mainpress;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Scanner;

import secureSign.mainpress.R;
import signswig.HMM;
import signswig.SWIGTYPE_p_p_int;
import signswig.signature;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

public class SecureSignActivity extends Activity {
	
	// LOAD C++ LIB
	static{
	    try { 
	        System.loadLibrary("swig");
	    } catch (UnsatisfiedLinkError e) {
	      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
	      System.err.println(Environment.getDataDirectory().getAbsolutePath());
	      System.exit(1);
	    }
	    catch (SecurityException e) {
		      System.err.println("Native code library failed to security. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
		      System.exit(1);
	    }
	}
	
	// Constantes
	static final int MAX_LOADSTRING = 100;
	static final int MIN_SIGN_LENTH	= 30;
	
	static final boolean LAST_STATE_EXIT = true;
	static final int N_VECTORS_GAUSSIAN = 30;	// nombre de gaussiennes par �tat, utiliser pour
										// chercher le nombre d'�tat optimal (d'apr�s BAO)
	static final int N_STATE_MIN = 2;
	static final int N_GAUSSIAN_MIN = 4;

	/* To control the Capture Dialog: capture the signature to train or to test...*/
	static final int CR_TRAIN = 1;
	static final int CR_TEST = 2;
	static final int CR_IDEN = 3;
	static final int CR_NOREASON = 0;
	static final int MAX_STRING_LENGTH = 256;
	// variables
	boolean		replayed;
	int			replayIndex;

	Editable		inputText;	// contain the new username


	/************ Signature data and Related functions  ********/
	static final int MAX_SIGNATURE_LENGTH =	4096;
	static final int Y_BASE = 27;
	static final int PENUP = 1;
	static final int PENDOWN = 2;
	static final int PENMOVE = 0;
	static final int LOOSE_MARKER =	5;
	static final int CAPTURE_TIMER = 10000;
	
	// MESSAGES :
	static final int MNO_USER = 0;
	static final int MRETRAIN = 1;
	static final int MDONT_EXIST = 2;
	static final int MDELETE = 3;
	private static final int MENTROP1 = 4;
	private static final int MENTROP2 = 5;
	String Mname = null;
	
	final Activity parentActivity = this;

	int	numberOut; // unsigned


	/* I had used a signature class to represent the signature, but
	when using it, the time to store coordinate was slower, because 
	we had to recompute the adress of each signature member...

	Thus I decided to use this way, not very good in notion of
	Oriented Object, but faster */

	int x; // unsigned
	int y; // unsigned



	private Spinner spinner_acceuil;
	
	@Override
	public void onRestart()
	{
		super.onRestart();
		try {
			AddListId();
		} catch (FileNotFoundException e) {
			// Auto-generated catch block
			e.printStackTrace();
		}
	}
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Singleton.getInstance().iden = new identities("");
        //init the identity object
    	String currentDir;
    	currentDir = this.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
    	String configFile = new String(currentDir);
    	configFile = configFile.concat("/identity.txt");
    	Singleton.getInstance().iden.setIdentityFile(configFile);
    	Singleton.getInstance().SIGNMAX = 3;

    	

    	
    	// Perform application initialization:
    	if (!InitInstance ()) 
    	{
    		finish();
    	}
    	
        final Button buttonnew_user = (Button) findViewById(R.id.buttonsmall);
        
       buttonnew_user.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Perform action on click new user
            	Singleton.getInstance().captureReason = CR_TRAIN;
        		Intent intent = new Intent(parentActivity, NewUserActivity.class);
        		startActivity(intent);   	
            }});
       
       final Button buttonretrain = (Button) findViewById(R.id.buttonretrain);
       buttonretrain.setOnClickListener(new View.OnClickListener() {
           public void onClick(View v) {
               // Perform action on click retrain
	           	if (spinner_acceuil.getSelectedItem() == null){
        	        showDialog(MNO_USER);
	           		return;
	           	}
	           	
	           	removeDialog(MRETRAIN);
	       		showDialog(MRETRAIN);
		        
           }});
      
        
        final Button buttonabout = (Button) findViewById(R.id.buttonabout);
        buttonabout.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Perform action on click about
        		Intent intent = new Intent(parentActivity, AboutActivity.class);
        		startActivity(intent);   	
            }}); 

//        final Button buttonverify = (Button) findViewById(R.id.buttonverify);
//        buttonverify.setOnClickListener(new View.OnClickListener() {
//            public void onClick(View v) {
//                // Perform action on click verify
//            	if (spinner_acceuil.getSelectedItem() != null){
//           			Singleton.getInstance().actualName = (String) spinner_acceuil.getSelectedItem();
//	               	Singleton.getInstance().actualCode = Singleton.getInstance().iden.getCode(Singleton.getInstance().actualName);
//	            	Singleton.getInstance().captureReason = CR_IDEN;
//	        		Intent intent = new Intent(parentActivity, VerifyActivity.class);
//	        		intent.addFlags( Intent.FLAG_ACTIVITY_CLEAR_TOP);
//	        		startActivity(intent);
//            	}
//            	else{
//            		showDialog(MNO_USER);
//            	}
//            }});
        
        final Button buttondelete = (Button) findViewById(R.id.buttondelete);
        buttondelete.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Perform action on click delete
            	if (spinner_acceuil.getSelectedItem() != null)
            		DeleteUser((String) spinner_acceuil.getSelectedItem());   	
            	else
            		showDialog(MNO_USER);
            	
            }});        

        final Button buttonentrop = (Button) findViewById(R.id.buttonentrop);
        buttonentrop.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Perform action on click delete
            	if (spinner_acceuil.getSelectedItem() != null){
            		String s = "Entropy =\t";
            		double tab = 0;
            		for (int i = 1; i <= Singleton.getInstance().SIGNMAX; ++i){
            		tab += testModel((String) spinner_acceuil.getSelectedItem(), i);   	
            		if (tab == -1)
            			return;
            		}
            		s += (String.valueOf(tab/Singleton.getInstance().SIGNMAX).substring(0, 4));
            		
    			    String dataPath = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
    				dataPath += "/database/";
    				dataPath += Singleton.getInstance().iden.getCode((String) spinner_acceuil.getSelectedItem());
    				
					File llfile = new File(dataPath + "/llfile.txt");
					try {
						Scanner sc = new Scanner(llfile);

					FileWriter writer = null;
					String texte = s + " bits/second";
					String s2 = "";
					if (llfile.exists())
						s2 += sc.nextLine() + "\n";
					
					llfile.delete();
					llfile.createNewFile();

						writer = new FileWriter(llfile, true);
						writer.write(s2, 0, s2.length());
						writer.write(texte,0,texte.length());

						writer.close();
					} catch (FileNotFoundException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					
            		if (tab/Singleton.getInstance().SIGNMAX < 9){
                		removeDialog(MENTROP2);
                		showDialog(MENTROP2);
            		}
            		else{
                		removeDialog(MENTROP1);
                		showDialog(MENTROP1);
            			
            		}
            	}
            	else{
            		showDialog(MNO_USER);
            	}
            	
            }});  
        
        final Button buttonexport = (Button) findViewById(R.id.buttonexport);
            buttonexport.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    // Perform action on click export
        			String state = Environment.getExternalStorageState();

        			if (!Environment.MEDIA_MOUNTED.equals(state)) {
        				return;
        			}
        			
        			
        			String wmyDocumentDir = Environment.getExternalStorageDirectory().getAbsolutePath();
        			if (!(new File(wmyDocumentDir)).exists())
        				return;
        			wmyDocumentDir += "/SecureSignDatabaseCopy/";
        			(new File(wmyDocumentDir)).mkdir();
        			
        			String source = new String(parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath() + "/database/");

        			try {
						copyDir(source, wmyDocumentDir);
					} catch (IOException e) {
						// Auto-generated catch block
						e.printStackTrace();
					}
                }});
        	}
	
	@Override 
	protected Dialog onCreateDialog(int id) { 
    AlertDialog alertDialog = null; 

	AlertDialog.Builder builder;
	LayoutInflater inflater;
	View layout;
	switch (id) {
	case MNO_USER:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView textno_user = (TextView) layout.findViewById(R.id.label);
    	textno_user.setText("No user selected!");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
	case MRETRAIN:
   		final String name = (String) spinner_acceuil.getSelectedItem();   	
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.basedialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView textretrain = (TextView) layout.findViewById(R.id.label);
    	textretrain.setText("Do you want to retrain " + name + "? \nOld files will be deleted!");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
            		(new File (parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath() + "/database/" + Singleton.getInstance().iden.getCode(name) + "/model.txt")).delete();
        	        Singleton.getInstance().captureReason = CR_TRAIN;
                   	Singleton.getInstance().actualName = (String) spinner_acceuil.getSelectedItem();
                   	Singleton.getInstance().actualCode = Singleton.getInstance().iden.getCode(Singleton.getInstance().actualName);
               		Singleton.getInstance().trainBaseIndex = 1;
                   	Intent intent = new Intent(parentActivity, TrainingActivity.class);
               		startActivity(intent);
            		dialog.cancel();
			        }
        })
        .setNegativeButton("No", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
	case MDONT_EXIST:	
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView textdont_exist = (TextView) layout.findViewById(R.id.label);
    	textdont_exist.setText("User does not exist!");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
	case MDELETE:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView textdelete = (TextView) layout.findViewById(R.id.label);
    	textdelete.setText("Delete " + Mname + "?");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
        		DeleteFiles(Mname);
        		dialog.cancel();
			        }
        })
        .setNegativeButton("No", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
    case MENTROP1: 
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.entrop2dialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView textentrop = (TextView) layout.findViewById(R.id.label);
    	textentrop.setText("Your signature has high entropy. It is vulnerable to attacks. You should change it by a more complex signature !");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
    case MENTROP2: 
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.entropdialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView textentrop2 = (TextView) layout.findViewById(R.id.label);
    	textentrop2.setText("Your signature has low entropy. It is resistant to attacks");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
        
    case 100: 

    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.accepteddialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView text = (TextView) layout.findViewById(R.id.label);
    	text.setText("SIGNATURE ACCEPTED");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break;
    case 200: 

    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView text2 = (TextView) layout.findViewById(R.id.label);
    	text2.setText("SIGNATURE REJETED");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
    case 300: 
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.basedialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView text3 = (TextView) layout.findViewById(R.id.label);
    	text3.setText("This is a signature message");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
   } 
    return alertDialog; 
}
        
    	
    	
    /**
    **  FUNCTION: initInstance()
    **  
    **  RETURN : true = OK, false = failure
    **
    **  USAGE : Init main window
    **
    */  
	boolean	InitInstance ()
	{
		String hmmConfigFile = this.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
		hmmConfigFile += "/config.ini";

		try {
			ReadHmmConfiguration(hmmConfigFile);
		} catch (IOException e2) {
			// Auto-generated catch block
			e2.printStackTrace();
		}
		
        setContentView(R.layout.main);
        spinner_acceuil = (Spinner) findViewById(R.id.spinner_accueil);
        try {
			Singleton.getInstance().iden.loadIdentities();
		} catch (FileNotFoundException e1) {
			// Auto-generated catch block
			e1.printStackTrace();
		}
        
		try {
			AddListId();
		} catch (FileNotFoundException e) {
			// Auto-generated catch block
			e.printStackTrace();
		}
		
		return true;
		
		
	}

	/**
	**  FUNCTION: AddListId()
	**  
	**  USAGE : Spinner update
	*/  
	void AddListId() throws FileNotFoundException
	{
        int j = 0;
        String actual = Singleton.getInstance().actualName;
		String[] s = new String[Singleton.getInstance().iden.getNumberIden()];
		for(int i = 0; i < Singleton.getInstance().iden.getNumberIden(); i ++){
			s[i] = Singleton.getInstance().iden.getName(i + 1);
			if (s[i].equals(actual))
				j = i;
		}
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, s); 
        spinner_acceuil.setAdapter(adapter);
        spinner_acceuil.setSelection(j);
	}

	/**
	**  FUNCTION: DeleteUser()
	**  
	**  PARAM : an identity name
	**  
	**  USAGE : Delete the identity
	*/  
	void DeleteUser(final String name) 
	{
		if(Singleton.getInstance().iden.getCode(name) == null)
    		showDialog(MDONT_EXIST);
	    else{
	    	Mname = null;
	    	Mname = name;
	    	removeDialog(MDELETE);
	    	showDialog(MDELETE);
	    }
	}
		
	/**
	**  FUNCTION: DeleteFiles()
	**  
	**  PARAM : an identity name
	**  
	**  USAGE : Delete the identity files
	*/  
	void DeleteFiles(String name)
	{
		//delete the directory
		String dataPath = this.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
		dataPath += "/database/";
		dataPath += Singleton.getInstance().iden.getCode(name);
		File dir = new File(dataPath);
		String[] s = dir.list();
		
		for (int i = 0; i < s.length; i++) {
		    (new File(s[i])).delete();
		}		    
		dir.delete();

		Singleton.getInstance().iden.delIdentity(Singleton.getInstance().iden.getCode(name)); /// le actualCode n'est pas encore mis � jour
		try {
			Singleton.getInstance().iden.saveIdentities();
		} catch (IOException e) {
			// Auto-generated catch block
			e.printStackTrace();
		}
		//update the commbo box, the actual code and name...
		try {
			AddListId();
		} catch (FileNotFoundException e) {
			// Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
	/**
	**  FUNCTION: ReadHmmConfiguration()
	**  
	**  PARAM : the configuration file filename
	**  
	**  USAGE : Read the configurations data
	*/  
	void ReadHmmConfiguration(String filename) throws IOException
	{
		File ifs = new File(filename);

//		if (!ifs.exists()){
			//TCHAR m_legende[125] = TEXT("Ne pas pouvoir lire le fichier \"config.ini\".\nV�rifiez s'il existe ou.\n Utilisez la fonction \"Config\" pour le cr�er");
			//MessageBox(hDlg, m_legende, TEXT("INFO"), MB_OK);
			//SendDlgItemMessage (NULL, STATIC_INFORMATION, WM_SETTEXT, (LONG)(LPCSTR)m_legende, 0);
			//Create file configuration with the default value
			//Singleton.getInstance().SIGNMAX = 3;
			Singleton.getInstance().STATE = 3;
			Singleton.getInstance().GAUSSIAN = 4;
			Singleton.getInstance().THRESHOLD = 0.65;
			ifs.createNewFile();
			ifs.setWritable(true);
			
			
			FileWriter writer = new FileWriter(ifs, true);
			writer.write(String.valueOf(Singleton.getInstance().SIGNMAX) + "\n");
			writer.write(String.valueOf(Singleton.getInstance().STATE) + "\n");
			writer.write(String.valueOf(Singleton.getInstance().GAUSSIAN) + "\n");
			writer.write(String.valueOf(Singleton.getInstance().THRESHOLD) + "\n");
		
			writer.write("\n\n\n#Ce fichier contient les informations importantes pour le bon fonctionnement du \n");
			writer.write("#programme. Ne r�digez pas si vous ne comprenez pas!\n\n");
			writer.write("#Les param�tres sont:\n");
			writer.write("#	nombre de signatures pour l'entrainement\n");
			writer.write("#	nombre d'�tat par d�faut\n");
			writer.write("#	nombre de gaussiennes par �tat\n");
			writer.write("#	seuil de d�cision\n\n");
			writer.write("#Auteur: Ly Van Bao\n");
			
			writer.close();
			return;
//		}else{
//			Scanner scanner = new Scanner(ifs);
//
//			if (scanner.hasNextLine())
//				Singleton.getInstance().SIGNMAX = Integer.valueOf(scanner.nextLine());
//			if (scanner.hasNextLine())
//				Singleton.getInstance().STATE = Integer.valueOf(scanner.nextLine());
//			if (scanner.hasNextLine())
//				Singleton.getInstance().GAUSSIAN = Integer.valueOf(scanner.nextLine());
//			if (scanner.hasNextLine())
//				Singleton.getInstance().THRESHOLD = Double.valueOf(scanner.nextLine());
//
//			scanner.close();
//		}
	}
	
	/**
	**  FUNCTION: copyDir() and copyDirectory
	**  
	**  PARAM : the source directory
	**          the destination directory
	**  
	**  USAGE : Copy all files from source to dest
	*/  
	public void copyDir(String source, String dest) throws IOException{
		File src = new File(source);
		File dst = new File(dest);
		copyDirectory(src, dst);
	}
	
	public void copyDirectory(File srcPath, File dstPath) throws IOException{
		if (srcPath.isDirectory())
		{
			if (!dstPath.exists())
			{
				dstPath.mkdir();
			}

			String files[] = srcPath.list();
			for(int i = 0; i < files.length; i++)
			{
				copyDirectory(new File(srcPath, files[i]), new File(dstPath, files[i]));
			}
		}
		else
		{
			if(!srcPath.exists())
			{
				return;
			}
			else
			{
				InputStream in = new FileInputStream(srcPath);
		        OutputStream out = new FileOutputStream(dstPath);
    
				// Transfer bytes from in to out
		        byte[] buf = new byte[1024];
				int len;
		        while ((len = in.read(buf)) > 0) {
					out.write(buf, 0, len);
				}
				in.close();
		        out.close();
			}
		}
	}
	
	
	void signProcess(signature ob){
		ob.setIndex(0, 1, 2, 3, -1, -1, -1);
		ob.interpolateCoordinates();
		ob.filter();
		ob.centralizeCoordinates();
		ob.parameterizeXY();	
	
	}
	
	double testModel(String user, int i)
	{
		Singleton.getInstance().actualName = user;
		Singleton.getInstance().actualCode = Singleton.getInstance().iden.getCode(user);
		String dataPath = this.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
		dataPath += "/database/";
		dataPath += Singleton.getInstance().actualCode;
		String modelFile = new String(dataPath);
		modelFile += "/model2.txt";

		File f = new File(modelFile);
		if (!f.exists()){
	        showDialog(MDONT_EXIST);
				return -1;
		}
	//// HMM for verification 
		HMM hmm = new HMM(modelFile, false);

		if(!hmm.isInit()){
	        showDialog(MDONT_EXIST);
				return -1;
		}
		signature ob = new signature(dataPath + "/sign" + String.valueOf(i) + ".txt", 4);
		if (ob.isInit()){
			signProcess(ob);
			hmm.normalizeObsParameters(ob);	// If the HMM did not use this functionality
												// there will have no problem.
			SWIGTYPE_p_p_int stSeq = hmm.getInitInt();
			hmm.getStateNumber();
			int T = ob.getObsSize();

			hmm.viterbi(ob, stSeq, SecureSignActivity.LAST_STATE_EXIT);

			//double dtwDis = 0;


			for(int k = 0; k < hmm.getObsNumber(); k++){
				hmm.getSeqLenK(k);
				hmm.scomputeViterbiHamDistance(stSeq, T, 
					hmm.getSeqK(k), hmm.getSeqLenK(k));	// cette distance est d�j� normalis�e par N
			}

			final double entrop=hmm.entropySwig(ob, stSeq)*100/T;
			

	/// End of HMM verification code ///


	// HMM need it	
			//Inform the user about the scores in the main interface
       		return entrop;
 


		}else{
			return -1;
		}
	}
}