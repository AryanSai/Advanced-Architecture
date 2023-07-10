import java.lang.Exception;
import java.lang.Long;
import java.lang.String;
import java.lang.System;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.Reader;

class Simulator
{
    
    public static void simulate(InputStream incomingStream, PrintStream outputStream) throws Exception 
    {
        // See the documentation to understand what these variables mean.
        long microOpCount;
        long instructionAddress;
        long sourceRegister1;
        long sourceRegister2;
        long destinationRegister;
        String conditionRegister;
        String TNnotBranch;
        String loadStore;
        long immediate;
        long addressForMemoryOp;
        long fallthroughPC;
        long targetAddressTakenBranch;
        String macroOperation;
        String microOperation;

        long totalMicroops = 0;
        long totalMacroops = 0;
        
        BufferedReader r = new BufferedReader(new InputStreamReader(incomingStream));
        String line;
        
        outputStream.format("Processing trace...\n");
        
        while (true) {
            line = r.readLine();
            if (line == null) {
                break;
            }
            String [] tokens = line.split("\\s+");
            
            microOpCount = Long.parseLong(tokens[0]);
            instructionAddress = Long.parseLong(tokens[1], 16);
            sourceRegister1 = Long.parseLong(tokens[2]);
            sourceRegister2 = Long.parseLong(tokens[3]);
            destinationRegister = Long.parseLong(tokens[4]);
            conditionRegister = tokens[5];
            TNnotBranch = tokens[6];
            loadStore = tokens[7];
            immediate = Long.parseLong(tokens[8]);
            addressForMemoryOp = Long.parseLong(tokens[9], 16);
            fallthroughPC = Long.parseLong(tokens[10], 16);
            targetAddressTakenBranch = Long.parseLong(tokens[11], 16);
            macroOperation = tokens[12];
            microOperation = tokens[13];
            
            // For each micro-op
            totalMicroops++;

            // For each macro-op
            if (microOpCount == 1) {
                totalMacroops++;
            }
        }
        
        outputStream.format("Processed %d trace records.\n", totalMicroops);
        
        outputStream.format("Micro-ops: %d\n", totalMicroops);
        outputStream.format("Macro-ops: %d\n", totalMacroops);
    }
    
    public static void main(String[] args) throws Exception
    {
        InputStream inputStream = System.in;
        PrintStream outputStream = System.out;
        
        if (args.length >= 1) {
            inputStream = new FileInputStream(args[0]);
        }
        
        if (args.length >= 2) {
            outputStream = new PrintStream(args[1]);
        }
        
        Simulator.simulate(inputStream, outputStream);
    }
}
