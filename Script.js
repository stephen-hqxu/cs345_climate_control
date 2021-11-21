//auto-fail time in ms
TIMEOUT(3600000);

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