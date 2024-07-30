#include "DAQApp.h"
#include "MonitorView.h"

#include "TGFrame.h"

const double GUI_REFRESH_RATE = 60.; // Hz

using namespace DAQ;

// TODO: Where do we tie everything together? It might be nice to have setup
//       for each system in main, so that main provides a record of everything
//       that's going on.
//       Systems that need to be tied together:
//         -- GUI
//         -- Data capture/processing
//         -- Data aggregator modules
//         -- Config parser
//         -- Logging

// TODO: Keeping everything self-contained under MonitorView allows us to
//       easily incorporate it into a larger application. We just have to
//       attach MonitorView to something in the larger app and voila.
//         -- So then we want everything to fall under MonitorView, and
//            we want the Application logic to be separate. So e.g. we
//            don't want the UI loop in MonitorView. We want a separate
//            'Application' class that doesn't rely on MonitorView directly.
//               > Maybe also make MonitorView not a TGMainFrame, and just
//                 attach it to a TGMainFrame.

// TODO: We might be able to run with a 'tie everything together in Main'
//       scheme if we have main handle attaching things to the MonitorView
//       instead of attaching them in MonitorView itself.
//         -- This makes sense to me -- so e.g. the control buttons stand
//            alone as part of the data run system, and main attaches them
//            to the MonitorView. The data run system can even just expose
//            two separate text buttons, already hooked up, and main can
//            just decide where to put them.
//              > SharedState can expose the runView panel and Main can
//                place the panel in the MonitorView.
//         -- This loses the ability to just attach the DAQ Monitor to 
//            anything. But we can get it back easily enough by just
//            enclosing it in a function.
//         -- For now, just link everything together in MonitorView and place
//            things there. But this is something to consider later.
//              > This is how we tie everything together though. Systems expose
//                plug-and-play GUI elements, and higher level code just needs
//                to attach those elements to the MonitorView.
//                  >> How to do UI updates though? We can't push them down
//                     from above if the GUI elements are self-contained.
//                     But UI updates have to be synchronized with the event
//                     loop.
//                  >> Maybe we should still see the systems as being managed
//                     by higher level code, rather than standing alone.
//                     Higher level code needs to push update calls and 
//                     decide how to arrange view elements, so we can't make
//                     the higher level code completely agnostic to the
//                     systems it's managing.
//                       >>> Would be cool though if we could just plug in
//                           a system with CMake and it would automatically
//                           inject itself into the GUI. The closest we get
//                           to being able to do that is tab modules, but
//                           there we still need a manicured order of
//                           modules in the view menu, so again we can't
//                           make higher level code agnostic to it.
// So let's go with a design where systems present wired-up GUI elements to
// higher level code, and higher level code pushes updates down to systems.
//   -- Maybe we can make a standard interface for systems broader than the
//      one I designed for tab modules. A system can expose its GUI element
//      as just a TGFrame object, and provide an update() method for higher
//      code to call. So as far as higher code is aware, all it has to mess
//      with is a TGFrame and an update() method.
//      So we have an interface like this:
/*
class System {

public:

    virtual TGFrame *getView() = 0;

    virtual void update([new shared state]) = 0;

}
*/
// Systems inherit and implement this, and higher level code interacts with
// them as System pointers.
//   -- Good way to construct the systems in the first place? Can we keep
//      higher level code agnostic?
//        > Again, we can't keep higher level code completely agnostic, 
//          since it still has to put the right views in the right spots

int main() {

    // TODO: We'd like to make app a TGMainFrame, but we can't because we can't
    //       use things like gClient->GetRoot() until the app is constructed.

    App app("MiniDAQ Monitor");

    ///////////////////////////////////////////////////////////////////////////

    // Construct a window for the DAQMonitor to live in.
    TGMainFrame *frame = new TGMainFrame(gClient->GetRoot());

    // Build the Monitor GUI and attach it to the main frame.
    MonitorView *window = new MonitorView(frame);
    frame->AddFrame(window, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // Title, size, and display the GUI.
    frame->SetWindowName("MiniDAQ Monitor");
    frame->MapSubwindows();
    frame->Resize(frame->GetDefaultSize());
    frame->MapWindow();

    // Tell the application to stop when the window is closed.
    frame->Connect("CloseWindow()", "DAQ::App", &app, "stop()");

    ///////////////////////////////////////////////////////////////////////////

    // Run the UI loop, processing events at the specified refresh rate.
    // This will not return until the GUI is stopped.
    // The rest of the program's functionality will be driven by GUI events.
    app.run(GUI_REFRESH_RATE);

    // At this point, the GUI has been shut down, and we are free to clean up.

    // TODO: I don't fully understand ROOT's memory management scheme. Make
    //       sure everything is getting cleaned up.
    //         -- Valgrind says we're leaking memory, even if we call every
    ///           delete function we can find

    // Recursively delete the GUI frame and all of its children.
    frame->Cleanup();

    return 0;

}