package secureSign.mainpress;

import java.io.File;
import java.io.IOException;

import secureSign.mainpress.R;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

public class NewUserActivity extends Activity {

	int nb_sign;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
		setContentView(R.layout.newuser);
		
    	final Activity parentActivity = this;



		Spinner dropdown2 = (Spinner) findViewById(R.id.spinner2);
		String[] items2 = new String[]{"3","4","5", "6", "7","8"};

		ArrayAdapter<String> adapter2 = new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, items2);
		dropdown2.setAdapter(adapter2);

		dropdown2.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
			@Override
			public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
				switch (position) {
					case 0:
						nb_sign = 3;
						break;
					case 1:
						nb_sign = 4;
						break;
					case 2:
						nb_sign = 5;
						break;
					case 3:
						nb_sign = 6;
						break;
					case 4:
						nb_sign = 7;
						break;
					case 5:
						nb_sign = 8;
						break;

				}
			}

			@Override
			public void onNothingSelected(AdapterView<?> parent) {

			}
		});



		final Button buttonMedium = (Button) findViewById(R.id.buttonMedium);

		buttonMedium.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				// Perform action on click OK

				final EditText edittext = (EditText) findViewById(R.id.editTextname);
				String name = edittext.getText().toString();
				//v�rifier si ce nom existe???
				if ((!name.isEmpty()) && (Singleton.getInstance().iden.getCode(name) == null)){
					Singleton.getInstance().iden.addIdentity(name);
					try {
						Singleton.getInstance().iden.saveIdentities();
					} catch (IOException e) {
						// Auto-generated catch block
						e.printStackTrace();
					}
					Singleton.getInstance().actualCode = new String(Singleton.getInstance().iden.getCode(name));
					//cr�er les r�pertoires... pour sauvegarder les signatures (originales et extraites)
					String dirName = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
					dirName += "/database/";
					dirName += Singleton.getInstance().iden.getCode(name);

					(new File(dirName)).mkdirs();

					Singleton.getInstance().captureReason = SecureSignActivity.CR_TRAIN;
					Singleton.getInstance().SIGNMAX = nb_sign;
					Singleton.getInstance().trainBaseIndex= 1;
					Intent intent = new Intent(parentActivity, TrainingActivityMedium.class);
					startActivity(intent);

					finish();
				}else{ // IDNO
					showDialog(0);
				}
			}});

        final Button buttonOk = (Button) findViewById(R.id.buttonOk);

        buttonOk.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Perform action on click OK

            	final EditText edittext = (EditText) findViewById(R.id.editTextname);
				String name = edittext.getText().toString();
				//v�rifier si ce nom existe???
				if ((!name.isEmpty()) && (Singleton.getInstance().iden.getCode(name) == null)){
					Singleton.getInstance().iden.addIdentity(name);
					try {
						Singleton.getInstance().iden.saveIdentities();
					} catch (IOException e) {
						// Auto-generated catch block
						e.printStackTrace();
					}
					Singleton.getInstance().actualCode = new String(Singleton.getInstance().iden.getCode(name));
					//cr�er les r�pertoires... pour sauvegarder les signatures (originales et extraites)
					String dirName = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
					dirName += "/database/";
					dirName += Singleton.getInstance().iden.getCode(name);

					(new File(dirName)).mkdirs();

					Singleton.getInstance().captureReason = SecureSignActivity.CR_TRAIN;
					Singleton.getInstance().SIGNMAX = nb_sign;
					Singleton.getInstance().trainBaseIndex= 1;
	        		Intent intent = new Intent(parentActivity, TrainingActivity.class);
	        		startActivity(intent);

	        		finish();
	        	}else{ // IDNO
	        		showDialog(0);
				}

            }});




        final Button buttoncancel = (Button) findViewById(R.id.buttoncancel);

        buttoncancel.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Perform action on click cancel
        		finish();
            }});


		final Button buttonBig = (Button) findViewById(R.id.buttonBig);

		buttonBig.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				// Perform action on click OK

				final EditText edittext2 = (EditText) findViewById(R.id.editTextname);
				String name2 = edittext2.getText().toString();
				//v�rifier si ce nom existe???
				if ((!name2.isEmpty()) && (Singleton.getInstance().iden.getCode(name2) == null)){
					Singleton.getInstance().iden.addIdentity(name2);
					try {
						Singleton.getInstance().iden.saveIdentities();
					} catch (IOException e) {
						// Auto-generated catch block
						e.printStackTrace();
					}
					Singleton.getInstance().actualCode = new String(Singleton.getInstance().iden.getCode(name2));
					//cr�er les r�pertoires... pour sauvegarder les signatures (originales et extraites)
					String dirName2 = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
					dirName2 += "/database/";
					dirName2 += Singleton.getInstance().iden.getCode(name2);

					(new File(dirName2)).mkdirs();

					Singleton.getInstance().captureReason = SecureSignActivity.CR_TRAIN;
					Singleton.getInstance().SIGNMAX = nb_sign;
					Singleton.getInstance().trainBaseIndex= 1;
					Intent intent2 = new Intent(parentActivity, TrainingActivityBig.class);
					startActivity(intent2);

					finish();
				}else{ // IDNO
					showDialog(0);
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
       
    case 0: 
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	TextView text2 = (TextView) layout.findViewById(R.id.label);
    	text2.setText("Please try another user name! User name exists or not valid!");
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

}
