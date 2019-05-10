package secureSign.mainpress;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.Region;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

import signswig.HMM;
import signswig.signature;

public class TrainingActivityBig extends Activity {



	    /** Menu ID for the command to clear the window. */
	    private static final int CLEAR_ID = Menu.FIRST;
	    private static final int SAVE_ID = Menu.FIRST + 1;
	    private static final int CANCEL_ID = Menu.FIRST + 2;
	    private static final int REPLAY_ID = Menu.FIRST + 3;
	    private long time;
    	final Activity parentActivity = this;
    	int replay_id = 0;

	    /** The view responsible for drawing the window. */
	    TrainingView TView;


	    // MESSAGES :
		private static final int MSIGN_NO = 0;
		private static final int MSIGN_COR = 1;
		private static final int MTRAINING = 2;
		private static final int MNO_SIGN = 3;
		private static final int MSIGN_START = 4;
		private static final int MMAX_LENGTH = 5;
		private static final int MMIN_LENGTH = 6;
		private static final int MOPEN_FILE = 7;
		private static final int MTRAINED = 8;
		private static final int MNOTRAIN = 9;
		String Mmessage;

    	@Override
    	protected Dialog onCreateDialog(int id) {
        AlertDialog alertDialog = null;

    	AlertDialog.Builder builder;
    	LayoutInflater inflater;
    	View layout;
    	TextView text;
    	switch (id) {
    	case MSIGN_NO:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.centerdialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("Signature " + String.valueOf(Singleton.getInstance().trainBaseIndex));
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
    	case MSIGN_COR:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.rejeteddialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("Please sign correctly your signature " + String.valueOf(Singleton.getInstance().trainBaseIndex));
        	builder = new AlertDialog.Builder(this);
        	builder.setView(layout);
        	builder
            .setCancelable(true)
            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                	TView.StartCapture();
                	dialog.cancel();
 			        }
            });
        	alertDialog = builder.create();

        	alertDialog.show();
        	alertDialog.setCancelable(false);
        	break;
    	case MTRAINING:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.centerdialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("The training will take a few seconds. Click on Ok and wait, please!");
        	builder = new AlertDialog.Builder(this);
        	builder.setView(layout);
        	builder
            .setCancelable(true)
            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
					try {
						TView.trainModel();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}		// Thread is better in this case
					//http://www.geocities.com/SiliconValley/Heights/6038/dthreads.html
	                dialog.cancel();
 			        }
            });
        	alertDialog = builder.create();

        	alertDialog.show();
        	alertDialog.setCancelable(false);
        	break;
    	case MNO_SIGN:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.rejeteddialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("No signature to replay !");
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
    	case MSIGN_START:


        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.centerdialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("Please enter your " + Singleton.getInstance().SIGNMAX + " signatures");
        	builder = new AlertDialog.Builder(this);
        	builder.setView(layout);
        	builder
            .setCancelable(true)
            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                	 TView.StartCapture();
                     dialog.cancel();
 			        }
            });
        	alertDialog = builder.create();

        	alertDialog.show();
        	alertDialog.setCancelable(false);
        	break;


    	case MMAX_LENGTH:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.rejeteddialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("Max signature length!");
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
    	case MMIN_LENGTH:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.rejeteddialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("Signature too small");
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
    	case MOPEN_FILE:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.rejeteddialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("can't open the file for writing");
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
    	case MTRAINED:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.basedialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText(Mmessage);
        	builder = new AlertDialog.Builder(this);
        	builder.setView(layout);
        	builder
            .setCancelable(true)
            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                	parentActivity.finish();
                	dialog.cancel();
 			        }
            });
        	alertDialog = builder.create();

        	alertDialog.show();
        	alertDialog.setCancelable(false);
        	break;
    	case MNOTRAIN:
        	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        	layout = inflater.inflate(R.layout.rejeteddialog,
        	                               (ViewGroup) findViewById(R.id.layout_root));

        	text = (TextView) layout.findViewById(R.id.label);
        	text.setText("Can't train the client model! Signature too short or too strange");
        	builder = new AlertDialog.Builder(this);
        	builder.setView(layout);
        	builder
            .setCancelable(true)
            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                	parentActivity.finish();
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


//    private Paint mPaint;
//
//	@Override
//	protected void onCreate(Bundle savedInstanceState) {
//		super.onCreate(savedInstanceState);
//		setContentView(new TrainingView(this));
//
//		mPaint = new Paint();
//		mPaint.setAntiAlias(true);
//		mPaint.setDither(true);
//		mPaint.setColor(0xFFFF0000);
//		mPaint.setStyle(Paint.Style.STROKE);
//		mPaint.setStrokeJoin(Paint.Join.ROUND);
//		mPaint.setStrokeCap(Paint.Cap.ROUND);
//		mPaint.setStrokeWidth(12);
//	}
//



	    @Override protected void onCreate(final Bundle savedInstanceState) {
	        super.onCreate(savedInstanceState);


	    	        TView = new TrainingView(this);
	    	        setContentView(TView);
	    }

	    /**
	     **  FUNCTION: OnBackPressed()
	     **
	     **  USAGE : Delete every files if the training is not complete
	     **
	     */
	    @Override public void onBackPressed(){
    		// if user not created :
		    String dataPath = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
			dataPath += "/database/";
			dataPath += Singleton.getInstance().actualCode + "/";
			File f = new File(dataPath + "model.txt");
			if (!f.exists()){
				//delete the directory
				File dir = new File(dataPath);
				String[] s = dir.list();

				for (int i = 0; i < s.length; i++) {
				    String s2 = dataPath + s[i];
				    (new File(s2)).delete();
				}
				dir.delete();

				Singleton.getInstance().iden.delIdentity((Singleton.getInstance().actualCode)); /// le actualCode n'est pas encore mis � jour
				try {
					Singleton.getInstance().iden.saveIdentities();
				} catch (IOException e) {
					// Auto-generated catch block
					e.printStackTrace();
				}
			}

	    	super.onBackPressed();
	    }


	    //********************************************************************************/
	    //***************************** MENU INITIALIZATION ******************************/
	    //********************************************************************************/

	    @Override public boolean onCreateOptionsMenu(Menu menu) {
	        menu.add(0, CLEAR_ID, 0, "Capture");
	        menu.add(0, CANCEL_ID, 0, "Cancel");
	        menu.add(0, REPLAY_ID, 0, "replay");
	        menu.add(0, SAVE_ID, 0, "Save");
	        return super.onCreateOptionsMenu(menu);

	    }

	    @Override public boolean onPrepareOptionsMenu(Menu menu) {

	        return super.onPrepareOptionsMenu(menu);


	    }

	    @Override public boolean onOptionsItemSelected(MenuItem item) {

	        switch (item.getItemId()) {
	            case CLEAR_ID:
	                TView.clear();
	                TView.StartCapture();
	                return true;
	            case SAVE_ID:
	                TView.clear();
					// Stop and save the acquisition signature
	                TView.StopCapture();
	                // sauvegarder les signatures ici

	                File dataDir = this.getBaseContext().getDir("main", MODE_PRIVATE);

	                String dataPath = new String(dataDir.getAbsolutePath());
	                dataPath += "/database/";
					dataPath += Singleton.getInstance().actualCode;

					if (Singleton.getInstance().captureReason == SecureSignActivity.CR_TRAIN){

						String fileName = new String(dataPath);
						fileName += "/sign";

						fileName += String.valueOf(Singleton.getInstance().trainBaseIndex);
						fileName += ".txt";

						try {
							if(TView.SaveSignature(fileName)){
								Singleton.getInstance().trainBaseIndex++;
							    if (Singleton.getInstance().trainBaseIndex <= Singleton.getInstance().SIGNMAX){
							    	removeDialog(MSIGN_NO);
								    showDialog(MSIGN_NO);
							    }
							}else{
								removeDialog(MSIGN_COR);
								showDialog(MSIGN_COR);
							}
						} catch (IOException e) {
							// Auto-generated catch block
							e.printStackTrace();
						}

						if (Singleton.getInstance().trainBaseIndex > Singleton.getInstance().SIGNMAX){
							showDialog(MTRAINING);
						}else{
							TView.StartCapture();
						}
					}


	                return true;
	            case CANCEL_ID:
	                TView.DestroySignature();
	    		    dataPath = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
	    			dataPath += "/database/";
	    			dataPath += Singleton.getInstance().actualCode + "/";
	    			File f = new File(dataPath + "model.txt");
	    			if (!f.exists()){
	    				//delete the directory
	    				File dir = new File(dataPath);
	    				String[] s = dir.list();

	    				for (int i = 0; i < s.length; i++) {
	    				    String s2 = dataPath + s[i];
	    				    (new File(s2)).delete();
	    				}
	    				dir.delete();

	    				Singleton.getInstance().iden.delIdentity((Singleton.getInstance().actualCode)); /// le actualCode n'est pas encore mis � jour
	    				try {
	    					Singleton.getInstance().iden.saveIdentities();
	    				} catch (IOException e) {
	    					// Auto-generated catch block
	    					e.printStackTrace();
	    				}
	    			}
	                finish();
	                return true;
	            case REPLAY_ID:
	                TView.DestroySignature();
	                TView.clear();
	                TView.StopCapture();
	                if (Singleton.getInstance().trainBaseIndex > 1)
	                	TView.replay_sign();
	                else{
	                	showDialog(MNO_SIGN);
	                }
	                return true;
	            default:
	                return super.onOptionsItemSelected(item);
	        }
	    }



	    //********************************************************************************/
	    //**************************** TRAININGVIEW = Paint ******************************/
	    //********************************************************************************/

	    public class TrainingView extends View {
	        // var interface
	    	private Bitmap mBitmap;
	        private Canvas mCanvas;
	        private final Rect mRect = new Rect();
	        private Path mPath;
			private Paint mBitmapPaint;
			private Paint mPaint;
	        private boolean mCurDown;
	        private int mCurX;
	        private int mCurY;
	        private float mCurPressure;
	        private float mCurSize;
	        private int mCurWidth;
	        public float predx;
	        public float predy;
	        public float predp;



			Display ecran = getWindowManager().getDefaultDisplay();
			int largeur= ecran.getWidth();
			int hauteur= ecran.getHeight();

			Region saveRegion = new Region((int) Math.rint(0.65*largeur), (int) Math.rint(0.75*hauteur), (int) Math.rint(0.8*largeur), (int) Math.rint(0.83*hauteur));
			//Region replayRegion = new Region(850, 1250, 1190, 1350);
			Region captureRegion = new Region((int) Math.rint(0.2*largeur), (int) Math.rint(0.75*hauteur), (int) Math.rint(0.35*largeur), (int) Math.rint(0.83*hauteur));






	        // values
	        private int counter;
	        private Point coordinates[];
	        private long timeStamp[];
	        private float pressureArray[];
	        private Integer numberGottenPoints[];
	        private Integer penStatus[];
	        private boolean stop;

	        // replay
	        private boolean replay = false;
	        private int i_replay;
	        private int lcounter;
			int lcoorx[];
			int lcoory[];
			int ltimes[];
			float lpress[];
			Thread thread;
			boolean kill;

			//thread
			public class DrawReplay implements Runnable {




			    /**
			     **  FUNCTION: replay()
			     **
			     **  USAGE : replay the sign data in a different thread to draw it
			     **
			     */
		        public void replay(){
		        	if (kill){
		                mBitmapPaint.setARGB(255, 0, 0, 0);

		                mCanvas.drawPaint(mBitmapPaint);
		                postInvalidate();
		                return;
		        	}
		        	if (replay){
		        		if(i_replay < lcounter){
		        			if (lcoory[i_replay] == 0 && lcoorx[i_replay] == 0){
		        				predx = -1;
		        				predy = -1;
		        				predp = -1;
		        			}
		        			else{
		        				drawPoint(lcoorx[i_replay], lcoory[i_replay], lpress[i_replay], 1, false);
		        				try {
		        					Thread.currentThread();
		        					Thread.sleep(ltimes[i_replay]);
		        				} catch (InterruptedException e) {
		        					// Auto-generated catch block
		        					e.printStackTrace();
		        				}
		        			}
		        			i_replay++;
		        		}
		        		else{
		        	        i_replay = 0;
		        	        lcounter = 0;
		        			lcoorx = null;
		        			lcoory = null;
		        			ltimes = null;
		        			lpress = null;
		        			replay = false;
		        			StopCapture();
		        		}
		        	}
		        }

				public void run() {
					while (replay){
						replay();
					}
				}

			};







	        public TrainingView(Context c) {


	            super(c);


	            mPath = new Path();

	            thread = null;
	            kill = false;
	            predx = -1;
	            predy = -1;
	            predp = -1;
                mBitmapPaint = new Paint();
                mBitmapPaint.setAntiAlias(true);




	            if (Singleton.getInstance().trainBaseIndex == 1)
	            {
	            	removeDialog(MSIGN_START);
	            	showDialog(MSIGN_START);
	            }

	        }

	        public void clear() {
	            if (mCanvas != null) {
                    mBitmapPaint.setARGB(255, 255, 255, 255);
	                mCanvas.drawPaint(mBitmapPaint);
	                invalidate();
	            }
	        }

	        @Override protected void onSizeChanged(int w, int h, int oldw,
	                int oldh) {
	            int curW = mBitmap != null ? mBitmap.getWidth() : 0;
	            int curH = mBitmap != null ? mBitmap.getHeight() : 0;
	            if (curW >= w && curH >= h) {
	                return;
	            }

	            if (curW < w) curW = w;
	            if (curH < h) curH = h;

				Bitmap newBitmap = Bitmap.createBitmap(curW, curH,
						Bitmap.Config.RGB_565);
				Canvas newCanvas = new Canvas(newBitmap);
				newCanvas.drawColor(0xffffffff);

				if (mBitmap != null) {
					newCanvas.drawBitmap(mBitmap, 0, 0, null);
				}
				mBitmap = newBitmap;
				mCanvas = newCanvas;
				StartCapture();
	        }

//            @Override
//            protected void onSizeChanged(int w, int h, int oldw, int oldh) {
//                super.onSizeChanged(w, h, oldw, oldh);
//                mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
//                mCanvas = new Canvas(mBitmap);
//                StartCapture();
//            }



	        @Override protected void onDraw(Canvas canvas) {

				canvas.drawColor(0xFFFFFFFF);
				mBitmapPaint.setTextSize(70);
				mBitmapPaint.setColor(Color.WHITE);
	            if (mBitmap != null) {
	                canvas.drawBitmap(mBitmap, 0, 0, mBitmapPaint);
	            }


// on récupère la taille de l'écran
				DisplayMetrics metrics = new DisplayMetrics();
				getWindowManager().getDefaultDisplay().getMetrics(metrics);

				int width = metrics.widthPixels;
				int height = metrics.heightPixels;



	// on crée les paints pour les rectangles

				Paint blue = new Paint();
				blue.setColor(0xFF5D8ACF);

				Paint noir = new Paint();
				noir.setColor(0xFF000000);

	// on crée les rectangles qui vont délimiter la zone de signature

//	//petit
//				Rect gauche = new Rect();
//				gauche.set(0,0,(int) Math.rint(0.4*width),(int) height);
//				canvas.drawRect(gauche, noir);
//
//				Rect droite = new Rect();
//				droite.set((int) Math.rint(0.6*width),0,width, (int) height);
//				canvas.drawRect(droite, noir);
//
//				Rect haut = new Rect();
//				haut.set(0,0, width,(int) Math.rint(0.3*height));
//				canvas.drawRect(haut, noir);
//
//				Rect bas = new Rect();
//				bas.set(0,(int) Math.rint(0.5*height),width,(int) height);
//				canvas.drawRect(bas, noir);

//	//moyen
//				Rect gauche = new Rect();
//				gauche.set(0,0,(int) Math.rint(0.3*width),(int) height);
//				canvas.drawRect(gauche, noir);
//
//				Rect droite = new Rect();
//				droite.set((int) Math.rint(0.7*width),0,width, (int) height);
//				canvas.drawRect(droite, noir);
//
//				Rect haut = new Rect();
//				haut.set(0,0, width,(int) Math.rint(0.2*height));
//				canvas.drawRect(haut, noir);
//
//				Rect bas = new Rect();
//				bas.set(0,(int) Math.rint(0.6*height),width,(int) height);
//				canvas.drawRect(bas, noir);
//

//grand
				Rect gauche = new Rect();
				gauche.set(0,0,(int) Math.rint(0.2*width),(int) height);
				canvas.drawRect(gauche, noir);

				Rect droite = new Rect();
				droite.set((int) Math.rint(0.8*width),0,width, (int) height);
				canvas.drawRect(droite, noir);

				Rect haut = new Rect();
				haut.set(0,0, width,(int) Math.rint(0.1*height));
				canvas.drawRect(haut, noir);

				Rect bas = new Rect();
				bas.set(0,(int) Math.rint(0.7*height),width,(int) height);
				canvas.drawRect(bas, noir);


	//régions pour appuyer sur les boutons


				Rect saveRect = new Rect();
				saveRect.set((int) Math.rint(0.65*width), (int) Math.rint(0.75*height), (int) Math.rint(0.8*width), (int) Math.rint(0.83*height));
				canvas.drawRect(saveRect, blue);
				canvas.drawText("Save",(float) 0.689*width, (float) 0.805*height, mBitmapPaint);

	//			Rect replayRect = new Rect();
	//			replayRect.set(850, 1250, 1190, 1350);
	//			canvas.drawRect(replayRect, blue);
	//			canvas.drawText("Replay",920, 1323, mBitmapPaint);

				Rect captureRect = new Rect();
				captureRect.set((int) Math.rint(0.2*width), (int) Math.rint(0.75*height), (int) Math.rint(0.35*width), (int) Math.rint(0.83*height));
				canvas.drawRect(captureRect, blue);
				canvas.drawText("Clear",(float) 0.234*width, (float) 0.805*height, mBitmapPaint);




		}






	        @Override public boolean onTouchEvent(MotionEvent event) {



	        	// nothing during replay
	        	if (replay)
	        		return true;
	            if (stop == true)
	            	return true;

				float pointx = event.getX();
				float pointy = event.getY();

				if (saveRegion.contains((int) pointx, (int) pointy))
				{
					TView.clear();
					// Stop and save the acquisition signature
					TView.StopCapture();
					// sauvegarder les signatures ici

					File dataDir = parentActivity.getBaseContext().getDir("main", MODE_PRIVATE);

					String dataPath = new String(dataDir.getAbsolutePath());
					dataPath += "/database/";
					dataPath += Singleton.getInstance().actualCode;

					if (Singleton.getInstance().captureReason == SecureSignActivity.CR_TRAIN){

						String fileName = new String(dataPath);
						fileName += "/sign";

						fileName += String.valueOf(Singleton.getInstance().trainBaseIndex);
						fileName += ".txt";

						try {
							if(TView.SaveSignature(fileName)){
								Singleton.getInstance().trainBaseIndex++;
								if (Singleton.getInstance().trainBaseIndex <= Singleton.getInstance().SIGNMAX){
									removeDialog(MSIGN_NO);
									showDialog(MSIGN_NO);
								}
							}else{
								removeDialog(MSIGN_COR);
							}
						} catch (IOException e) {
							// Auto-generated catch block
							e.printStackTrace();
						}

						if (Singleton.getInstance().trainBaseIndex > Singleton.getInstance().SIGNMAX){
							showDialog(MTRAINING);
						}else{
							TView.StartCapture();
						}
					}


					return true;
				}

//				if (replayRegion.contains((int) pointx, (int) pointy))
//				{
//					TView.DestroySignature();
//					TView.replay_sign();
//					return true;
//				}


				if (captureRegion.contains((int) pointx, (int) pointy))
				{
					TView.clear();
					TView.StartCapture();
					return true;
				}




	            int action = event.getAction();
	            mCurDown = action == MotionEvent.ACTION_DOWN
	                    || action == MotionEvent.ACTION_MOVE;
	            int N = event.getHistorySize();

	            for (int i=0; i<N; i++) {
    				timeStamp[counter] = System.currentTimeMillis() - time;
    				coordinates[counter].x = (int) event.getHistoricalX(i);
    				coordinates[counter].y = (int) event.getHistoricalY(i);
    				pressureArray[counter] = event.getHistoricalPressure(i);
    				numberGottenPoints[counter] = 1;
    	            if (action == MotionEvent.ACTION_DOWN){
        				penStatus[counter] = SecureSignActivity.PENDOWN;
    	            	predx = -1;
    	            	predy = -1;
    	            	predp = -1;
    	            }
    	            else if (action == MotionEvent.ACTION_UP){
        				coordinates[counter].x = 0;
        				coordinates[counter].y = 0;
        				pressureArray[counter] = 0;
        				numberGottenPoints[counter] = 0;
        				penStatus[counter] = SecureSignActivity.PENUP;
    	            }
    	            else {
    	            	penStatus[counter] = SecureSignActivity.PENMOVE;
    	            }

    				drawPoint(event.getHistoricalX(i), event.getHistoricalY(i),
	                        event.getHistoricalPressure(i),
	                        event.getHistoricalSize(i), true);
    				counter++;

    				if(counter == SecureSignActivity.MAX_SIGNATURE_LENGTH){
    					showDialog(MMAX_LENGTH);
    					StopCapture();
    					return true;
    				}
	            }

				timeStamp[counter] = System.currentTimeMillis() - time;
				coordinates[counter].x = (int) event.getX();
				coordinates[counter].y = (int) event.getY();
				pressureArray[counter] = event.getPressure();
				numberGottenPoints[counter] = 1;
	            if (action == MotionEvent.ACTION_DOWN){
    				penStatus[counter] = SecureSignActivity.PENDOWN;
	            	predx = -1;
	            	predy = -1;
	            	predp = -1;
	            }
	            else if (action == MotionEvent.ACTION_UP){
    				coordinates[counter].x = 0;
    				coordinates[counter].y = 0;
    				pressureArray[counter] = 0;
    				numberGottenPoints[counter] = 0;
    				penStatus[counter] = SecureSignActivity.PENUP;
	            }
	            else {
	            	penStatus[counter] = SecureSignActivity.PENMOVE;
	            }

	            drawPoint(event.getX(), event.getY(), event.getPressure(),
	                    event.getSize(), true);
	            ++counter;

	            if(counter == SecureSignActivity.MAX_SIGNATURE_LENGTH){
			        showDialog(MMAX_LENGTH);
					StopCapture();
				}

	            return true;
	        }

		    /**
		     **  FUNCTION: DrawPoint()
		     **
		     **  PARAM : the finger position.x
		     **          the finger position.y
		     **          the finger pressure
		     **          the finger size
		     **
		     **  USAGE : draw every points between the new position and the last one, if there was any
		     **          else, draw a point at the new position
		     **
		     */
	        private void drawPoint(float x, float y, float pressure, float size, boolean from) {
	            if (predx != -1)
	            	// points entre pr�c�dent et actuel
	            	for (float mul=(float) 0.02; mul<1; mul += 0.02)
	                {
	                    mCurX = (int) (predx + mul * (x - predx));
	                    mCurY = (int) (predy + mul * (y - predy));
	                    mCurPressure = (predp + mul * (pressure - predp));
	    				if (parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT ||
	    					parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT)

	    					mCurSize = (float) 0.005;
	    				else
	    					mCurSize = (float) 0.005;
	    				mCurWidth = (int)(mCurSize*(getWidth()/3));

	                    if (mCurWidth < 1) mCurWidth = 1;
	                    if ((mCurDown || replay) && mBitmap != null) {
	                    	int pressureLevel = (int)(mCurPressure*255);

	                    	// ICI ON CHANGE LA COULEUR DE LA SIGNATURE



                            mBitmapPaint.setARGB(255, 0, 0, 0);
                            mBitmapPaint.setStrokeWidth(12000);

	                    	mCanvas.drawCircle(mCurX, mCurY, mCurWidth, mBitmapPaint);
	                    	mRect.set(mCurX-mCurWidth-2, mCurY-mCurWidth-2,
	                    			  mCurX+mCurWidth+2, mCurY+mCurWidth+2);
	                    	if (from)
	                    		invalidate(mRect);
	                    	else
	                    		postInvalidate();
	                    }
	                }

	            // point actuel
	                mCurX = (int)x;
	                mCurY = (int)y;
	                mCurPressure = pressure;
    				if (parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT ||
	    					parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT)
	    					mCurSize = (float) 0.005;
	    				else
	    					mCurSize = (float) 0.005;
	                mCurWidth = (int)(mCurSize*(getWidth()/3));
	                predx = x;
	                predy = y;
	                predp = pressure;
	                if (mCurWidth < 1) mCurWidth = 1;
	                if ((mCurDown || replay) && mBitmap != null) {
	                	int pressureLevel = (int)(mCurPressure*255);

                        mBitmapPaint.setARGB(pressureLevel, 0, 0, 0);

	                	mCanvas.drawCircle(mCurX, mCurY, mCurWidth, mBitmapPaint);
	                	mRect.set(mCurX-mCurWidth-2, mCurY-mCurWidth-2,
	                			  mCurX+mCurWidth+2, mCurY+mCurWidth+2);
                    	if (from)
                    		invalidate(mRect);
                    	else
                    		postInvalidate();
	                }

	        }

		    /**
		     **  FUNCTION: StartCapture()
		     **
		     **  USAGE : prepare every data for the drawing
		     **
		     */
		    private boolean StartCapture(){

		    	// kill replay
		    	if (thread != null){
		    		kill = true;
		    		try {
		    			// wait for clean
						Thread.currentThread();
						Thread.sleep(50);
					} catch (InterruptedException e) {
						// Auto-generated catch block
						e.printStackTrace();
					}
		    		thread.stop();
		    		thread = null;
		    		try {
		    			// wait for clear
						Thread.currentThread();
						Thread.sleep(50);
					} catch (InterruptedException e) {
						// Auto-generated catch block
						e.printStackTrace();
					}
		    	}
		    	kill = false;
		        i_replay = 0;
		        lcounter = 0;
				lcoorx = null;
				lcoory = null;
				ltimes = null;
				lpress = null;
				replay = false;
		    	//numberOut = unsigned(conf.sampling_rate * conf.time_out); � voir
	        	invalidate(mRect);
	        	stop = false;
		    	/************ Init the signature data ************/
		    	int i;
		    	counter = 0;
		    	DestroySignature();	// destroy firstly the signature data
		    	coordinates = new Point[SecureSignActivity.MAX_SIGNATURE_LENGTH + 1];
		    	timeStamp = new long[SecureSignActivity.MAX_SIGNATURE_LENGTH + 1];
				pressureArray = new float[SecureSignActivity.MAX_SIGNATURE_LENGTH + 1];
		    	numberGottenPoints = new Integer[SecureSignActivity.MAX_SIGNATURE_LENGTH + 1];
		    	penStatus = new Integer[SecureSignActivity.MAX_SIGNATURE_LENGTH + 1];

		    	for(i = 0; i < SecureSignActivity.MAX_SIGNATURE_LENGTH+1; i ++){
		    		coordinates[i] = new Point(0, 0);
					pressureArray[i] = 0;
		    		timeStamp[i] = new Integer(0);
		    		numberGottenPoints[i] = new Integer(0);
		    		penStatus[i] = new Integer(-1);
		    	}
		    	/************************************************/
		    	time = System.currentTimeMillis();
		    	return true;
		    }

		    /**
		     **  FUNCTION: StopCapture()
		     **
		     **  USAGE : Block the drawing
		     **
		     */
		    void StopCapture(){
		    	stop = true;
		    }

		    /**
		     **  FUNCTION: DestroySignature()
		     **
		     **  USAGE : Delete the signature data
		     **
		     */
			void DestroySignature(){
					coordinates = null;
					timeStamp = null;
					pressureArray = null;
					numberGottenPoints = null;
					penStatus = null;
					counter = 0;
			}

		    /**
		     **  FUNCTION: SaveSignature()
		     **
		     **  PARAM : the signX.txt filename
		     **
		     **  USAGE : Save the signature data in fname, if the signature is accepted
		     **
		     */
			boolean SaveSignature(String fname) throws IOException
			{
				int i, j, begin, end;
				if(coordinates == null)
					return false; //no signature to save
				//pre-process data: set coordinate (0, 0) to all point between
				//PENUP and PENDOWN
				begin = 0;
				do{
					// Find the PENUP
					while(!(penStatus[begin] == SecureSignActivity.PENUP)){
						begin++;
						if(begin == counter || counter == 0){ // no good signature
							DestroySignature();/*
	    			        builder.setMessage("PENDOWN SANS PENUP")
	    		               .setCancelable(true)
	    		               .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
	    		                   public void onClick(DialogInterface dialog, int id) {
	    		                        dialog.cancel();
	    		                   }
	    		               });
	    			        AlertDialog alert = builder.create();
	    			        alert.setCancelable(false);
	    			        alert.show();*/
							return false;
						}
					}

					i = begin;
					//put 0 to coordinates after PENUP and before PENDOWN
					while((!(penStatus[i] == SecureSignActivity.PENDOWN)) && (i < counter)){
						coordinates[i].x = 0;
						coordinates[i].y = 0;
						pressureArray[i] = 0;
						penStatus[i] = SecureSignActivity.PENUP;
						numberGottenPoints[i] = 0;
						i++;
					}
					// all points that preced a PENUP that have numberGottenPoints==0
					// are considered as PENUP..
					j = begin;
					while((numberGottenPoints[j] == 0)
						|| (numberGottenPoints[j] == SecureSignActivity.LOOSE_MARKER)){
						penStatus[j] = SecureSignActivity.PENUP;
						j--;
					}
					// Find onother PENUP...
					begin = i;
				}while(begin < counter);

				//find the begining and end points of signature
				begin = 0;		//effet de bord
				end = counter - 1;
				while(begin < end){
					if(numberGottenPoints[++begin] != 0)		break;
				}
				while(end > begin){
					if(numberGottenPoints[--end] != 0)			break;
				}

				if((end - begin) < SecureSignActivity.MIN_SIGN_LENTH){
					DestroySignature();
					showDialog(MMIN_LENGTH);
					return false;
				}

				File sfile = new File(fname);
				Singleton.getInstance().CreateDirIfNeeded(fname);


				if (sfile.exists())
					sfile.delete();
				sfile.createNewFile();
				if(sfile.exists() && sfile.canWrite()){ // file opened OK
					int lastSavePointIndex = begin; //before first point: nothing
					for (i = begin; i <= end; i++)
					{
						if(penStatus[i] == SecureSignActivity.PENMOVE){ // save only these points
							//first, make (0,0) to point that have
							//numberGottenPoints == LOOSE_MARKER
							if(numberGottenPoints[i] == SecureSignActivity.LOOSE_MARKER){
								numberGottenPoints[i] = 0;
								coordinates[i].x = 0;
								coordinates[i].y = 0;
								pressureArray[i] = 0;
							}
							// save one sample point
							if (!sfile.canWrite())
								sfile.setWritable(true);

							//pressureArray[i] � print
							FileWriter writer = null;
							String texte = String.valueOf(coordinates[i].x) + "\t" +
										   String.valueOf(coordinates[i].y) + "\t" +
										   String.valueOf(timeStamp[i] - timeStamp[lastSavePointIndex]) + "\t" +
										   String.valueOf(pressureArray[i]) + "\n";


							writer = new FileWriter(sfile, true);
							writer.write(texte,0,texte.length());

							writer.close();

							lastSavePointIndex = i;
						}
						else if (penStatus[i] == SecureSignActivity.PENDOWN){
							//first, make (0,0) to point that have
							//numberGottenPoints == LOOSE_MARKER
							if(numberGottenPoints[i] == SecureSignActivity.LOOSE_MARKER){
								numberGottenPoints[i] = 0;
								coordinates[i].x = 0;
								coordinates[i].y = 0;
								pressureArray[i] = 0;
							}
							// save one sample point
							if (!sfile.canWrite())
								sfile.setWritable(true);


							FileWriter writer = null;
							writer = new FileWriter(sfile, true);
							String texte = "0" + "\t" +
									   "0" + "\t" +
									   String.valueOf(timeStamp[i] - timeStamp[lastSavePointIndex]) + "\t" +
									   "0" + "\n";

							//pressureArray[i] � print
							writer.write(texte,0,texte.length());
							texte = null;
							texte = String.valueOf(coordinates[i].x) + "\t" +
										   String.valueOf(coordinates[i].y) + "\t" +
										   String.valueOf(timeStamp[i] - timeStamp[lastSavePointIndex]) + "\t" +
										   String.valueOf(pressureArray[i]) + "\n";

							writer.write(texte,0,texte.length());

							writer.close();

							lastSavePointIndex = i;
						}
					}
					DestroySignature();
					return true;
				}

					// can not open the file for writing:
					// normally
				showDialog(MOPEN_FILE);
					DestroySignature();
					return false;
			}


			/**
			 * @param b ************************************************************************************/

			void signProcess(signature ob, boolean b){
				ob.setIndex(0, 1, 2, 3, -1, -1, -1);
				ob.interpolateCoordinates();
				ob.filter();
				ob.centralizeCoordinates();
				if (!b)
				ob.parameterize();
				else
					ob.parameterizeXY();

			}

		    /**
		     **  FUNCTION: trainModel()
		     **
		     **  USAGE : Train the program with the SIGNMAX signatures
		     * @throws IOException
		     **
		     */
			void trainModel() throws IOException
			{

			    String dataPath = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
				dataPath += "/database/";
				dataPath += Singleton.getInstance().actualCode;
				int counter = 0;

				signature obs[] = null;
				String trainFiles[] = new String[Singleton.getInstance().SIGNMAX];
				for (int i = 0; i < Singleton.getInstance().SIGNMAX; i ++){
					String signfile = new String(dataPath);
					signfile += "/sign";

					signfile += String.valueOf(i + 1);
					signfile += ".txt";

					File f = new File(signfile);
					Singleton.getInstance().CreateDirIfNeeded(signfile);
					if (f.exists()){
						trainFiles[counter] = new String(signfile);
						counter ++;
					}
				}
				obs = new signature[Singleton.getInstance().SIGNMAX]; //indice commence par 0
				int K = loadObsFile(trainFiles, Singleton.getInstance().SIGNMAX, obs, false);

			////// This code uses HMM //////

				int k;
				double len = 0;

				int statesNumber, gaussiansNumber; //nombre de gaussiennes et d'�tats

				for(k = 0; k < K; k++){
					len += obs[k].getObsSize();
				}
				// Compute the optimal number of states
				gaussiansNumber = 4;
				statesNumber = (int) (len / SecureSignActivity.N_VECTORS_GAUSSIAN / gaussiansNumber );
				if(statesNumber < SecureSignActivity.N_STATE_MIN)
					statesNumber = SecureSignActivity.N_STATE_MIN;	//Minimum 2 states

				//priorities of the HMM
				int transitionType = Singleton.TRANS_LR;
				int covMatrixType = Singleton.COV_DIAG;
				//boolean bHmmRandomInit = true;
				boolean bParaNorm = Singleton.PARAMETER_NORMALIZATION_YES;
				double paraStandardDiviation = 2;
				double endAccuracy = 0.0;		// we use the end accuracy to stop the training or
				int endIterationNumber = 6;		//we fix the number of iteration

				//create the HMM
				HMM hmm = new HMM(statesNumber, gaussiansNumber, transitionType, covMatrixType,
					false, bParaNorm, paraStandardDiviation, endAccuracy, endIterationNumber);


				//(Obs **) ++ est diff�rent de (signature **) ++; attention
				hmm.getInitObs(k);
				for(k = 0; k < K; k++){
					hmm.initKObs(k, obs[k]);
				}
				//assign training database
				hmm.assignObs(K); //This function accepts only Obs

				boolean hmmOK = hmm.train();

				while(!hmmOK && (hmm.getStateNumber() > SecureSignActivity.N_STATE_MIN)){
					hmm.setNStates(hmm.getStateNumber() - 1);
					hmmOK = hmm.train();
				}

				if(hmmOK){
					String modelfile = new String(dataPath);
					modelfile += "/model.txt";
					hmm.save(modelfile, false);
					//Inform the user about the new model
					trainEntrop();

					File llfile = new File(dataPath + "/llfile.txt");
					FileWriter writer = null;
					String texte = "Model likelihood = " + String.valueOf(hmm.getLLba()).substring(0, 5);


					writer = new FileWriter(llfile, true);
					writer.write(texte,0,texte.length());

					writer.close();

					Mmessage = null;
					Mmessage = new String("Your model is trained and saved in User" + (Singleton.getInstance().actualCode).substring(2, (Singleton.getInstance().actualCode).length()) + " directory.\nThe average likelihood of your signatures is:   " + String.valueOf(hmm.getLLba()).substring(0, 5) + "\n");
					removeDialog(MTRAINED);
					showDialog(MTRAINED);
				}else{
					showDialog(MNOTRAIN);
				}
			}

		    /**
		     **  FUNCTION: LoadObsFile()
		     **
		     **  PARAM : a filenames list
		     **          the number of files
		     **          an initialized signature array
		     **
		     **  USAGE : Load signatures from filesname[]
		     * @param b
		     **
		     */
			int loadObsFile(String filenames[], int nfiles, signature obs[], boolean b)
			{
				int K = 0; // compteur des observations r�ellement existantes
				for(int i = 0; i < nfiles; i++){
					signature otmp = new signature(filenames[i], 4);
					if (otmp.isInit()){
						(obs)[K] = new signature(filenames[i], 4 );
						signProcess(obs[K], b);
						K ++;
					}
				}
				return K;
			}

		    /**
		     **  FUNCTION: replay_sign()
		     **
		     **  USAGE : Prepare the replay of an old signature
		     **
		     */
	        boolean replay_sign(){
	        	String fname;
	        	File fin;

			    String dataPath = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
				dataPath += "/database/";
				dataPath += Singleton.getInstance().actualCode + "/";

	        	replay_id++;
	        	if (replay_id >= Singleton.getInstance().trainBaseIndex){
	        		replay_id = 1;
	        	}

	        	fname = dataPath + "/sign" + String.valueOf(replay_id) + ".txt";

	        	fin = new File(fname);
	        	if(!fin.exists()){
	        		return true;
	        	}else{

	        		// Clear the screen
		            StartCapture();

		            lcoorx = new int[SecureSignActivity.MAX_SIGNATURE_LENGTH+1];
	        		lcoory = new int[SecureSignActivity.MAX_SIGNATURE_LENGTH+1];
	        		ltimes = new int[SecureSignActivity.MAX_SIGNATURE_LENGTH+1];
	        		lpress = new float[SecureSignActivity.MAX_SIGNATURE_LENGTH+1];
	        		lcounter = 0;


		            Scanner sc;
					try {
						sc = new Scanner(fin);
		        		while (sc.hasNextInt()){
		        			lcoorx[lcounter] = sc.nextInt();
		        			lcoory[lcounter] = sc.nextInt();
		        			ltimes[lcounter] = sc.nextInt();
		        			lpress[lcounter] = Float.valueOf(sc.next());
		        			++lcounter;
		        		}

		        		sc.close();
		        	} catch (FileNotFoundException e1) {
						// Auto-generated catch block
						e1.printStackTrace();
					}

	        		//draw signature
		            int i_replay=1;
		            predx = -1;
		            predy = -1;
		            predp = -1;
	        		while(lcoory[i_replay] == 0)	i_replay++;

	        		replay = true;
	        		DrawReplay drawReplay = new DrawReplay();

	        		thread = new Thread(drawReplay);
	        		thread.start();

	        	}
	        	return true;


	        }

			void trainEntrop()
			{
				//double entrop = 0;
			    String dataPath = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
				dataPath += "/database/";
				dataPath += Singleton.getInstance().actualCode;
				int counter = 0;

				signature obs[] = null;
				String trainFiles[] = new String[Singleton.getInstance().SIGNMAX];
				for (int i = 0; i < Singleton.getInstance().SIGNMAX; i ++){
					String signfile = new String(dataPath);
					signfile += "/sign";

					signfile += String.valueOf(i + 1);
					signfile += ".txt";

					File f = new File(signfile);
					Singleton.getInstance().CreateDirIfNeeded(signfile);
					if (f.exists()){
						trainFiles[counter] = new String(signfile);
						counter ++;
					}
				}
				obs = new signature[Singleton.getInstance().SIGNMAX]; //indice commence par 0
				int K = loadObsFile(trainFiles, Singleton.getInstance().SIGNMAX, obs, true);

			////// This code uses HMM //////

				int k;
				double len = 0;

				int statesNumber, gaussiansNumber; //nombre de gaussiennes et d'�tats

				for(k = 0; k < K; k++){
					len += obs[k].getObsSize();
				}
				// Compute the optimal number of states
				gaussiansNumber = 4;
				statesNumber = (int) (len / SecureSignActivity.N_VECTORS_GAUSSIAN / gaussiansNumber );
				if(statesNumber < SecureSignActivity.N_STATE_MIN)
					statesNumber = SecureSignActivity.N_STATE_MIN;	//Minimum 2 states

				//priorities of the HMM
				int transitionType = Singleton.TRANS_LR;
				int covMatrixType = Singleton.COV_DIAG;
				//boolean bHmmRandomInit = true;
				boolean bParaNorm = Singleton.PARAMETER_NORMALIZATION_YES;
				double paraStandardDiviation = 2;
				double endAccuracy = 0.0;		// we use the end accuracy to stop the training or
				int endIterationNumber = 6;		//we fix the number of iteration

				//create the HMM
				HMM hmm = new HMM(statesNumber, gaussiansNumber, transitionType, covMatrixType,
					false, bParaNorm, paraStandardDiviation, endAccuracy, endIterationNumber);


				//(Obs **) ++ est diff�rent de (signature **) ++; attention
				hmm.getInitObs(k);
				for(k = 0; k < K; k++){
					hmm.initKObs(k, obs[k]);
				}
				//assign training database
				hmm.assignObs(K); //This function accepts only Obs

				boolean hmmOK = hmm.train();

				while(!hmmOK && (hmm.getStateNumber() > SecureSignActivity.N_STATE_MIN)){
					hmm.setNStates(hmm.getStateNumber() - 1);
					hmmOK = hmm.train();
				}

				if(hmmOK){
					String modelfile = new String(dataPath);
					modelfile += "/model2.txt";
					hmm.save(modelfile, false);

				}
			}

	    }

}


