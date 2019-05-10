package secureSign.mainpress;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.Date;
import java.util.Scanner;

import signswig.HMM;
import signswig.SWIGTYPE_p_p_int;
import signswig.signature;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.Region;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Environment;
import android.support.constraint.solver.widgets.Rectangle;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class VerifyActivity extends Activity//implements View.OnTouchListener
 {
    


    /** Menu ID for the command to clear the window. */
    private static final int CLEAR_ID = Menu.FIRST;
    private static final int SAVE_ID = Menu.FIRST + 1;
    private static final int CANCEL_ID = Menu.FIRST + 2;
    private static final int REPLAY_ID = Menu.FIRST + 3;
    private long time;
	final Activity parentActivity = this; 
	int replay_id = 0;
	
    /** The view responsible for drawing the window. */
    VerifyView TView;

    // MESSAGES :
	private static final int MSIGN_COR = 1;
	private static final int MDONT_EXIST = 2;
	private static final int MSIGN_START = 4;
	private static final int MMAX_LENGTH = 5;
	private static final int MMIN_LENGTH = 6;
	private static final int MOPEN_FILE = 7;
	private static final int MSCORE = 8;
	private static final int MACCEPTED = 9;
	private static final int MREJETED = 10;
	String Mmessage;
    
	@Override 
	protected Dialog onCreateDialog(int id) { 
    AlertDialog alertDialog = null; 

	AlertDialog.Builder builder;
	LayoutInflater inflater;
	View layout;
	TextView text;


	switch (id) {
	case MSIGN_COR:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog2,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	text = (TextView) layout.findViewById(R.id.label);
    	text.setText("Please resign correctly");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
            	TView.StartCapture();
            	dialog.cancel();
				TView.clear();
				TView.StartCapture();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
	case MDONT_EXIST:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog2,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	text = (TextView) layout.findViewById(R.id.label);
    	text.setText("Your model does not exist !");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                dialog.cancel();

				parentActivity.finish();
				TView.clear();
				TView.StartCapture();
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
    	text.setText("Please sign to verify your identity!");
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
	case MMAX_LENGTH:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog2,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	text = (TextView) layout.findViewById(R.id.label);
    	text.setText("Max signature length !");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                 dialog.cancel();
				TView.clear();
				TView.StartCapture();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break; 
	case MMIN_LENGTH:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog2,
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
				TView.clear();
				TView.StartCapture();
			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);
    	break;
	case MOPEN_FILE:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog2,
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
	case MSCORE:
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
            	dialog.cancel();
            	//parentActivity.finish();


				TView.clear();
				TView.StartCapture();

			        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);

    	break;
	case MACCEPTED:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.accepteddialog2,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	text = (TextView) layout.findViewById(R.id.label);
    	text.setText("SIGNATURE ACCEPTED");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
		.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
            	dialog.cancel();
            	removeDialog(MSCORE);
            	showDialog(MSCORE);
        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	
    	alertDialog.setCancelable(false);

    	break;
	case MREJETED:
    	inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
    	layout = inflater.inflate(R.layout.rejeteddialog2,
    	                               (ViewGroup) findViewById(R.id.layout_root));

    	text = (TextView) layout.findViewById(R.id.label);
    	text.setText("SIGNATURE REJECTED");
    	builder = new AlertDialog.Builder(this);
    	builder.setView(layout);
    	builder
        .setCancelable(true)
        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
            	dialog.cancel();
            	removeDialog(MSCORE);
            	showDialog(MSCORE);


        }
        });
    	alertDialog = builder.create();
    	
    	alertDialog.show();
    	alertDialog.setCancelable(false);

//		Intent intent = new Intent(parentActivity, VerifyActivity.class);
//		startActivity(intent);
    	break;
	}

    return alertDialog;

}

    @Override protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
    	        TView = new VerifyView(this);
    	        setContentView(TView);
    }

    //********************************************************************************/
    //***************************** MENU INITIALIZATION ******************************/
    //********************************************************************************/

//    @Override public boolean onCreateOptionsMenu(Menu menu) {
//        menu.add(0, CLEAR_ID, 0, "Capture");
//        menu.add(0, CANCEL_ID, 0, "Cancel");
//        menu.add(0, REPLAY_ID, 0, "replay");
//        menu.add(0, SAVE_ID, 0, "Save");
//
//        return super.onCreateOptionsMenu(menu);
//
//    }
//
//    @Override public boolean onPrepareOptionsMenu(Menu menu) {
//		return super.onPrepareOptionsMenu(menu);
//
//    }
//
//    @Override public boolean onOptionsItemSelected(MenuItem item) {
//        switch (item.getItemId()) {
//            case CLEAR_ID:
//                TView.clear();
//                TView.StartCapture();
//                return true;
//            case SAVE_ID:
//                TView.clear();
//				// Stop and save the acquisition signature
//                TView.StopCapture();
//
//                String testFile = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
//				//dans ce cas l�, la signature est acquise pour tester
//				testFile += "/database/" + Singleton.getInstance().actualCode; // modif...
//				testFile += "/test" + String.valueOf(Singleton.getInstance().iden.getNextTestSigIndex(Singleton.getInstance().actualCode)) + ".txt";
//
//			try {
//				if(TView.SaveSignature(testFile)){
//					Singleton.getInstance().iden.setNextTestSigIndex(Singleton.getInstance().actualCode);
//					Singleton.getInstance().iden.saveIdentities();
//					TView.testModel(testFile);
//				}else
//					showDialog(MSIGN_COR);
//			} catch (IOException e) {
//				// Auto-generated catch block
//				e.printStackTrace();
//			}
//
//
//
//				return true;
//            case CANCEL_ID:
//				Intent intent = new Intent(parentActivity, SecureSignActivity.class);
//				startActivity(intent);
//                return true;
//            case REPLAY_ID:
//                TView.DestroySignature();
//                TView.replay_sign();
//                return true;
//            default:
//                return super.onOptionsItemSelected(item);
//        }
//    }


    //********************************************************************************/
    //**************************** TRAININGVIEW = Paint ******************************/
    //********************************************************************************/


    public class VerifyView extends View {
        // var interface
    	private Bitmap mBitmap;
        private Canvas mCanvas;
        private final Rect mRect = new Rect();
		private Path mPath;
        private final Paint mPaint;
        private boolean mCurDown;
        private int mCurX;
        private int mCurY;
        private float mCurPressure;
        private float mCurSize;
        private int mCurWidth;
        private float predx;
        private float predy;
        private float predp;
        private Bitmap bitmapbutton;

		Display ecran = getWindowManager().getDefaultDisplay();
		int largeur= ecran.getWidth();
		int hauteur= ecran.getHeight();


		Region saveRegion = new Region((int) Math.rint(0.75*largeur), (int) Math.rint(0.75*hauteur), (int) Math.rint(0.9*largeur), (int) Math.rint(0.83*hauteur));
		Region replayRegion = new Region((int) Math.rint(0.425*largeur), (int) Math.rint(0.75*hauteur), (int) Math.rint(0.575*largeur), (int) Math.rint(0.83*hauteur));
		Region captureRegion = new Region((int) Math.rint(0.1*largeur), (int) Math.rint(0.75*hauteur), (int) Math.rint(0.25*largeur), (int) Math.rint(0.83*hauteur));


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

//		@Override
//		public boolean onTouchEvent(MotionEvent event) {
//			return gameThread.doOnTouch(event);
//		}
//
//
//		public boolean doOnTouch(MotionEvent event){
//
//			synchronized (event){
//
//				int eventAction = event.getAction();
//				int xEvent = (int)event.getX();
//				int yEvent = (int)event.getY();
//
//				switch (eventAction){
//
//					case MotionEvent.ACTION_DOWN:
//
//						if(xEvent >= rectangle.getLeft() && xEvent <= rectangle.getRight()
//								&& yEvent >= rectangle.getBottom() && yEvent<= rectangle.getTop())
//						{
//							touched = true;
//							score +=5;
//						}
//				}
//				break;
//				case MotionEvent.ACTION_UP:
//					touched = false;
//					break;
//			}
//			return true;
//		}
		
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
	                mPaint.setARGB(0xff, 255, 0, 0);
	                mCanvas.drawPaint(mPaint);
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
		
        public VerifyView(Context c) {
            super(c);
            mPath = new Path();
            thread = null;
            kill = false;
            predx = -1;
            predy = -1;
            predp = -1;
            mPaint = new Paint();
            mPaint.setAntiAlias(true);

            
            showDialog(MSIGN_START);
	        
            StartCapture();
        }

        public void clear() {
            if (mCanvas != null) {
                mPaint.setARGB(0xff, 255, 255, 255);
                mCanvas.drawPaint(mPaint);
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

        @Override protected void onDraw(Canvas canvas) {



// on récupère la taille de l'écran
			DisplayMetrics metrics = new DisplayMetrics();
			getWindowManager().getDefaultDisplay().getMetrics(metrics);

			int width = metrics.widthPixels;
			int height = metrics.heightPixels;



			canvas.drawColor(0xFFFFFFFF);
			mPaint.setStrokeWidth(2);
			mPaint.setTextSize(70);
			mPaint.setColor(Color.WHITE);

			if (mBitmap != null) {
				canvas.drawBitmap(mBitmap, 0, 0, mPaint);
			}

			Paint blue = new Paint();
			blue.setColor(0xFF5D8ACF);

			Paint noir = new Paint();
			noir.setColor(0xFF000000);

//			//petit
//			Rect gauche = new Rect();
//			gauche.set(0,0,(int) Math.rint(0.4*width),(int) height);
//			canvas.drawRect(gauche, noir);
//
//			Rect droite = new Rect();
//			droite.set((int) Math.rint(0.6*width),0,width, (int) height);
//			canvas.drawRect(droite, noir);
//
//			Rect haut = new Rect();
//			haut.set(0,0, width,(int) Math.rint(0.3*height));
//			canvas.drawRect(haut, noir);
//
//			Rect bas = new Rect();
//			bas.set(0,(int) Math.rint(0.5*height),width,(int) height);
//			canvas.drawRect(bas, noir);
//
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
			saveRect.set((int) Math.rint(0.75*width), (int) Math.rint(0.75*height), (int) Math.rint(0.9*width), (int) Math.rint(0.83*height));
			canvas.drawRect(saveRect, blue);
			canvas.drawText("Save",(float) 0.789*width, (float) 0.805*height, mPaint);

			Rect replayRect = new Rect();
			replayRect.set((int) Math.rint(0.425*width), (int) Math.rint(0.75*height), (int) Math.rint(0.575*width), (int) Math.rint(0.83*height));
			canvas.drawRect(replayRect, blue);
			canvas.drawText("Replay",(float) 0.45*width, (float) 0.805*height, mPaint);

			Rect captureRect = new Rect();
			captureRect.set((int) Math.rint(0.1*width), (int) Math.rint(0.75*height), (int) Math.rint(0.25*width), (int) Math.rint(0.83*height));
			canvas.drawRect(captureRect, blue);
			canvas.drawText("Clear",(float) 0.134*width, (float) 0.805*height, mPaint);



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

				String testFile = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
				//dans ce cas l�, la signature est acquise pour tester
				testFile += "/database/" + Singleton.getInstance().actualCode; // modif...
				testFile += "/test" + String.valueOf(Singleton.getInstance().iden.getNextTestSigIndex(Singleton.getInstance().actualCode)) + ".txt";

				try {
					if(TView.SaveSignature(testFile)){
						Singleton.getInstance().iden.setNextTestSigIndex(Singleton.getInstance().actualCode);
						Singleton.getInstance().iden.saveIdentities();
						TView.testModel(testFile);
					}else
						showDialog(MSIGN_COR);
				} catch (IOException e) {
					// Auto-generated catch block
					e.printStackTrace();
				}



				return true;
			}

			if (replayRegion.contains((int) pointx, (int) pointy))
			{
					TView.DestroySignature();
					TView.replay_sign();
					return true;
			}


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
	     **          is it a normal drawPoint (true) or launch from a replay thread (false)
	     **
	     **  USAGE : draw every points between the new position and the last one, if there was any
	     **          else, draw a point at the new position  
	     **
	     */  
        private void drawPoint(float x, float y, float pressure, float size, boolean from) {
            if (predx != -1)
        	for (float mul=(float) 0.02; mul<1; mul += 0.02)
            {
                mCurX = (int) (predx + mul * (x - predx));
                mCurY = (int) (predy + mul * (y - predy));
                mCurPressure = (predp + mul * (pressure - predp));
				if (parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT ||
    					parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT)
    					mCurSize = (float) 0.001;
    				else
    					mCurSize = (float) 0.001;
                mCurWidth = (int)(mCurSize*(getWidth()/3));
                if (mCurWidth < 1) mCurWidth = 1;
                if ((mCurDown || replay) && mBitmap != null) {
                	int pressureLevel = (int)(mCurPressure*255);
                	mPaint.setARGB(255, 0, 0, 0);
					mPaint.setStrokeWidth(2);
                	mCanvas.drawCircle(mCurX, mCurY, mCurWidth, mPaint);
                	mRect.set(mCurX-mCurWidth-2, mCurY-mCurWidth-2,
                			  mCurX+mCurWidth+2, mCurY+mCurWidth+2);
                	if (from)
                		invalidate(mRect);
                	else
                		postInvalidate();
                }
            }

            mCurX = (int)x;
            mCurY = (int)y;
            mCurPressure = pressure;
			if (parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT ||
					parentActivity.getResources().getConfiguration().orientation == ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT)
					mCurSize = (float) 0.001;
				else
					mCurSize = (float) 0.001;
			mCurWidth = (int)(mCurSize*(getWidth()/3));
            predx = x;
            predy = y;
            predp = pressure;
            if (mCurWidth < 1) mCurWidth = 1;
            if ((mCurDown || replay) && mBitmap != null) {
            	int pressureLevel = (int)(mCurPressure*255);
            	mPaint.setARGB(pressureLevel, 0, 0, 0);
				mPaint.setStrokeWidth(2);
            	mCanvas.drawCircle(mCurX, mCurY, mCurWidth, mPaint);
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
			pressureArray = new float[SecureSignActivity.MAX_SIGNATURE_LENGTH + 1];
	    	timeStamp = new long[SecureSignActivity.MAX_SIGNATURE_LENGTH + 1];
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
				pressureArray = null;
				timeStamp = null;
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
						
						// pressureArray[i] � ajouter
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


		/**************************************************************************************/
		
		void signProcess(signature ob){
			ob.setIndex(0, 1, 2, 3, -1, -1, -1);
			ob.interpolateCoordinates();
			ob.filter();
			ob.centralizeCoordinates();
			ob.parameterize();	
		
		}
		
		
	    /**
	     **  FUNCTION: LoadObsFile()
	     **  
	     **  PARAM : a filenames list
	     **          the number of files
	     **          an initialized signature array
	     **
	     **  USAGE : Load signatures from filesname[]
	     **
	     */  
		int loadObsFile(String filenames[], int nfiles, signature obs[])
		{
			int K = 0; // compteur des observations r�ellement existantes
			for(int i = 0; i < nfiles; i++){
				signature otmp = new signature(filenames[i], 4);
				if (otmp.isInit()){
					(obs)[K] = new signature(filenames[i], 4);
					signProcess(obs[K]);
					K ++;
				}
			}
			return K;
		}
		
	    /**
	     **  FUNCTION: testModel()
	     **  
	     **  PARAM : the test file
	     **
	     **  USAGE : Look if the testFile is similar as the model
	     **
	     */ 
		double testModel(String testFile)
		{
			String dataPath = parentActivity.getBaseContext().getDir("main", Context.MODE_PRIVATE).getAbsolutePath();
			dataPath += "/database/";
			dataPath += Singleton.getInstance().actualCode;
			//double entrop=0;
			String modelFile = new String(dataPath);
			modelFile += "/model.txt";

			File f = new File(modelFile);
			if (f.exists())
				System.out.println("test");
		//// HMM for verification 
			HMM hmm = new HMM(modelFile, false);

			if(!hmm.isInit()){
				showDialog(MDONT_EXIST);
					return -1;
			} 
			signature ob = new signature(testFile, 4);
			if (ob.isInit()){
				signProcess(ob);
				hmm.normalizeObsParameters(ob);	// If the HMM did not use this functionality
													// there will have no problem.
				SWIGTYPE_p_p_int stSeq = hmm.getInitInt();
				int N = hmm.getStateNumber();
				int T = ob.getObsSize();

				double LL = hmm.viterbi(ob, stSeq, SecureSignActivity.LAST_STATE_EXIT);
				double hamDis = 0;
				//double dtwDis = 0;

				double meanLenth = 0;
				for(int k = 0; k < hmm.getObsNumber(); k++){
					meanLenth += hmm.getSeqLenK(k);
					hamDis += hmm.scomputeViterbiHamDistance(stSeq, T, 
						hmm.getSeqK(k), hmm.getSeqLenK(k));	// cette distance est d�j� normalis�e par N
				}

				//entrop= hmm.entropySwig(ob, stSeq)*100/meanLenth;
				//entrop=hmm.entropy(hDlg,ob, stSeq);
				
/*
				WCHAR tmsg1[512];
				swprintf(tmsg1, TEXT("entropi = %4.2f\nmeanLenth = %4.2f"), entrop,meanLenth);
				MessageBox(hDlg,tmsg1 , TEXT("Decision"), MB_OK | MB_ICONINFORMATION);
*/



				stSeq = null;
				meanLenth /= hmm.getObsNumber();
				hamDis /= hmm.getObsNumber();

				if((meanLenth > 2*T) || (T > 2*meanLenth)){
					hamDis = Singleton.MAX_DOUBLE;
				}

				final double viterbiScore = java.lang.Math.exp(- hamDis * N / meanLenth);
				final double likelihoodScore = java.lang.Math.exp(-java.lang.Math.abs(LL - hmm.getLLba()));
				final double fusionScore = (viterbiScore + likelihoodScore) / 2;

		/// End of HMM verification code ///


				// Score message	
                Mmessage = null;
                Mmessage = new String("Likelihood score:\t\t" + String.valueOf(likelihoodScore + 0.001).substring(0, 4) + "\nViterbi score:\t\t" + String.valueOf(viterbiScore + 0.001).substring(0, 4) + "\nAverage score:\t\t" + String.valueOf(fusionScore + 0.001).substring(0, 4) + "\nDecision threshold:\t" + String.valueOf(Singleton.getInstance().THRESHOLD));
				//Inform the user about the scores in the main interface
				if(fusionScore >= Singleton.getInstance().THRESHOLD){
					showDialog(MACCEPTED);
				}else{
					showDialog(MREJETED);
				}

				try {
					createResultFile(105, Singleton.getInstance().actualName,(fusionScore >= Singleton.getInstance().THRESHOLD),fusionScore,"livraison","Pr�fecture de Bobigny");
				} catch (IOException e) {
					// Auto-generated catch block
					e.printStackTrace();
				}
				return fusionScore;
			}else{
				return -1;
			}
		}
		
	    /**
	     **  FUNCTION: createResultFile()
	     **  
	     **  PARAM : the sensorid value
	     **          the refered identity name
	     **          is the signature accepted true/false
	     **          the final test score
	     **          the signature context
	     **          where you sign
	     **
	     **  USAGE : Print every information from the test in the log file
	     **
	     */  
		void createResultFile(int sensorid, String name, boolean accept, double score, String context, String location) throws IOException{
			String state = Environment.getExternalStorageState();

			if (!Environment.MEDIA_MOUNTED.equals(state)) {
				return;
			}
			
			String timeString;
			String acceptString;
			Date currentTime = new Date(System.currentTimeMillis());
			
			if(accept){
				acceptString = "yes";
			}else{
				acceptString = "no";
			}
			timeString = "Android le ";
			timeString += currentTime.toString() + " : " + 
			String.valueOf((currentTime.getTime() - Date.valueOf(currentTime.toString()).getTime())/(1000 * 60 * 60)) + "h " +
			String.valueOf(((currentTime.getTime() - Date.valueOf(currentTime.toString()).getTime())/(1000 * 60)) % 60) + "m " +
			String.valueOf(((currentTime.getTime() - Date.valueOf(currentTime.toString()).getTime())/(1000)) % 60) + "s";
			
			String wmyDocumentDir = Environment.getExternalStorageDirectory().getAbsolutePath();
			if (!(new File(wmyDocumentDir)).exists())
				return;
			wmyDocumentDir += "/log";
			(new File(wmyDocumentDir)).mkdir();
			wmyDocumentDir += "/SecureSignLog.txt";

			
			File logFile = new File(wmyDocumentDir);
			if (!logFile.exists())
				logFile.createNewFile();
			

			FileWriter writer = new FileWriter(logFile, true);
			writer.write(String.valueOf(Singleton.getInstance().SIGNMAX) + "\n");
			writer.write("*sensorid* = " + String.valueOf(sensorid) + "\n");
			writer.write("*time* = " + String.valueOf(timeString) + "\n");
			writer.write("*name* = " + String.valueOf(name) + "\n");
			writer.write("*accept* = " + String.valueOf(acceptString) + "\n");
			writer.write("*score* = " + String.valueOf(score) + "\n");
			writer.write("*context* = " + String.valueOf(context) + "\n");
			writer.write("*location* = " + String.valueOf(location) + "\n");
			
			writer.close();

			return;
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
        	if (replay_id > Singleton.getInstance().SIGNMAX){
        		replay_id = replay_id - 5;
        	}
        	
        	fname = dataPath + "/sign" + String.valueOf(replay_id) + ".txt";

        	fin = new File(fname);
        	if(!fin.exists()){
        		return true;
        	}else{

        		// Clear the screen
	            clear();
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
    }
}


