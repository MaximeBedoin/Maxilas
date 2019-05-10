package secureSign.mainpress;

import secureSign.mainpress.R;
import android.app.Activity;
import android.os.Bundle;

public class AboutActivity extends Activity {

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // create the "about.xml" view
		setContentView(R.layout.about);
    }
}
