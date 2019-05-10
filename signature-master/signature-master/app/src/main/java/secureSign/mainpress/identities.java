package secureSign.mainpress;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;
import java.util.StringTokenizer;


public class identities {


	int dirId;
	int numberIden;
	
	class identity{
		identity prev;
		identity next;
		String name;
		String code;
		int nextTestSigIndex;
	
		identity(){
			name = null;
			code = null;
		}


	};

	
	private identity head;
	private identity queue;

	private String filename;
	

	//////////////////////////////////////////////////////////////////////
	// Construction
	//////////////////////////////////////////////////////////////////////

	identities(String filename_)
	{
		filename = new String(filename_);
		numberIden = 0;

		head = new identity();
		queue = new identity();

		head.prev = null;
		queue.next = null;
		head.next = queue;
		queue.prev = head;
		
		numberIden = 0;
	}


/**
**  FUNCTION: getName()
**  
**  PARAM : an identity code ("usX")
**  
**  RETURN : This identity name
*/  
	String getName(String code)
	{
		identity idtmp = head.next;
		
		while(idtmp != queue){
			if(idtmp.code.equals(code)){
				return idtmp.name;
			}
			idtmp = idtmp.next;
		}
		
		return null;
	} 

	/**
	**  FUNCTION: getCode()
	**  
	**  PARAM : an identity name
	**  
	**  RETURN : This identity code (usX)
	*/  
	String getCode(String name)
	{
		identity idtmp = head.next;
		
		while(idtmp != queue){
			if(idtmp.name.equals(name)){
				return idtmp.code;
			}
			idtmp = idtmp.next;
		}
		
		return null;
	}

	/**
	**  FUNCTION: getNumberIden()
	**  
	**  RETURN : NumberIden
	*/  
	int getNumberIden()
	{
		return numberIden;
	}

	/**
	**  FUNCTION: LoadIdentities()
	**  
	**  USAGE : Load the identities list from the identity filename
	*/  	
	void loadIdentities() throws FileNotFoundException
	{
		File idenFile = new File(filename);

		if(!idenFile.exists())
		{
		    System.out.println("Erreur de l'ouverture de fichier contenant les identit�s.");
			return;
		}

		Scanner scanner= new Scanner(idenFile);

		String line = null;
		if (scanner.hasNextLine())
			line = scanner.nextLine();
		
		dirId = Integer.valueOf(line);

		identity idtmp;
		while (scanner.hasNextLine()){
			line = scanner.nextLine();
			idtmp = new identity();

			


			
			//the name of identity
			StringTokenizer st = new StringTokenizer(line, "*-*" );
			  
			idtmp.name = new String(st.nextToken());

			//and the code of identity
			idtmp.code = new String(st.nextToken());

			//and the nextTestIndex to save all test signature
			idtmp.nextTestSigIndex = Integer.valueOf(st.nextToken());

			++numberIden;
			
			idtmp.next = queue;
			idtmp.prev = queue.prev;
			queue.prev.next = idtmp;
			queue.prev = idtmp;

		}
		idenFile = null;
	}

	/**
	**  FUNCTION: saveIdentities()
	**  
	**  USAGE : Write in <filename> the current existing identities
	*/  
	void saveIdentities() throws IOException
	{
		File idenFile = new File(filename);
		FileWriter writer = new FileWriter(filename);
		BufferedWriter out = new BufferedWriter(writer);
		
		if (!idenFile.exists())
		{
			idenFile.createNewFile();
			idenFile.setWritable(true);
		}
		
		if(!idenFile.exists() || !idenFile.canWrite())
		{
		    System.out.println("Erreur de l'ouverture de fichier contenant les identit�s.");
			System.exit(0);
		}

		identity idtmp;
		idtmp = head.next;
		out.write(String.valueOf(dirId) + "\n");
		
		if(head.next != queue){	// null identities
			do{
				out.write(idtmp.name + "*-*" + idtmp.code + "*-*" + 
				String.valueOf(idtmp.nextTestSigIndex) + "*-*\n");
				idtmp = idtmp.next;
			}while(idtmp != queue);
		}
		out.close();
		writer.close();
	}

	/**
	**  FUNCTION: addIdentity()
	**  
	**  PARAM : an identity name
	**  
	**  USAGE : Add a new identity in the identities' list
	*/  
	void addIdentity(String name)
	//le code est g�n�r� par le programme
	{
		identity idtmp = new identity();

		idtmp.nextTestSigIndex = 0;
		idtmp.name = new String(name);
		String code;
		String cDirId;

		cDirId = String.valueOf(dirId);

		code = new String("us" + cDirId);

		idtmp.code = new String(code);

		Singleton.getInstance().actualName = new String(name);
		Singleton.getInstance().actualCode = new String(code);

		numberIden++;
		dirId++;

		idtmp.next = queue;
		idtmp.prev = queue.prev;
		queue.prev.next = idtmp;
		queue.prev = idtmp;
	}

	/**
	**  FUNCTION: getName()
	**  
	**  PARAM : an identity position
	**  
	**  RETURN : an identity name
	**
	**  COMMENT : care with deleted identities
	*/  
	String getName(int position)
	{
		int postmp = 1;
		identity idtmp = head.next;
		while((postmp != position) & (postmp <= numberIden + 1)){
			idtmp = idtmp.next;
			postmp++;
		}

		if(postmp <= numberIden){
			return idtmp.name;
		}else{
			return null;
		}
	}

	/**
	**  FUNCTION: delIdentity()
	**  
	**  PARAM : an identity code ("usX")
	**  
	**  RETURN : 0 = ok, 1 = not found
	*/  
	int delIdentity(String code)
	{
		identity idtmp = head.next;
		
		while(idtmp != queue){
			if(idtmp.code.equals(code)){
				break;
			}
			idtmp = idtmp.next;
		}
		if(idtmp != queue){
			idtmp.next.prev = idtmp.prev;
			idtmp.prev.next = idtmp.next;
			idtmp = null;
			numberIden--;
			return 0;
		}else{
			return 1;
		}
	}

	/**
	**  FUNCTION: setName()
	**  
	**  PARAM : an identity name
	**          the new name for this identity
	**  
	**  RETURN : 0 = ok, 1 = not found
	*/  
	int setName(String oldName, String newName)
	{
		identity idtmp = head.next;
		
		while(idtmp != queue){
			if(idtmp.name.equals(oldName)){
				break;
			}
			idtmp = idtmp.next;
		}

		if(idtmp != queue){
			idtmp.name = null;
			idtmp.name = new String(newName);
			return 0;
		}else{
			return 1;
		}

	}

	/**
	**  FUNCTION: setIdentity()
	*/  
	boolean setIdentity(int index)
	{
		//probl�me ici 



		if((index < 1) | (index > numberIden)){
			return false;
		}else{
			
			identity idtmp = head.next;
			int counter = 1;
			while(counter < index){
				counter++;
				idtmp = idtmp.next;
			}
			Singleton.getInstance().actualName = new String(idtmp.name);
			Singleton.getInstance().actualCode = new String(idtmp.code);
			return true;
		}
	}

	/**************************************************************************************/
	/***************                    USEFULL                    ************************/
	/**************************************************************************************/

	int getNextTestSigIndex(String code)
	{
		identity idtmp = head.next;
		while(idtmp != queue){
			if(idtmp.code.equals(code)){
				return idtmp.nextTestSigIndex;
			}
			idtmp = idtmp.next;
		}
		return -1;	
	}

	void setNextTestSigIndex(String code)
	{
		identity idtmp = head.next;
		while(idtmp != queue){
			if(idtmp.code.equals(code)){
				idtmp.nextTestSigIndex ++;
				return;
			}
			idtmp = idtmp.next;
		}
	}

	void setIdentityFile(String filename_)
	{
		filename = null;
		filename = new String(filename_);
	}


	
}
