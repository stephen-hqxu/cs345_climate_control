//auto-fail time in ms
TIMEOUT(16200000);

//import java package
var package = new JavaImporter(java.io);
//scope of the import
with(package){
    var output = new Object();

    while(true){
        var idStr = id.toString();
        //check if output file is created
        if(!output[idStr]){
            output[idStr] = new FileWriter("mote_" + id + ".log");
            //log the position of this new node
            var pos = mote.getInterfaces().getPosition();
            output[idStr].write(time + ' ' + "[Simulation Script] Mote global position: (" + 
                pos.getXCoordinate() + ", " + 
                pos.getYCoordinate() + ", " + 
                pos.getZCoordinate() + ")\n");
        }
        //write to file
        output[idStr].write(time + ' ' + msg + "\n");

        //terminate simulation
        try{
            YIELD();

        }catch(e){
            for(var ids in output){
                output[ids].close();
            }

            throw("Test Ended");
        }
    }
}
