import com.bigswitch.vpi.IVirtualPortInterfaceFactory;
import com.bigswitch.vpi.IVirtualPortInterface;
import com.bigswitch.vpi.native_.NativeVPIFactory;

public class TestJVPI
{
    IVirtualPortInterfaceFactory factory = new NativeVPIFactory();

    public void info(String str)
    {
        System.out.println(str);
    }
    public void fail(String str)
    {
        System.out.println("FAIL: " + str);
        System.exit(1);
    }
    public void pass()
    {
        System.out.println("PASS");
        System.exit(1);
    }

    public boolean LoopbackTest()
    {
        info("Running Loopback Test");
        IVirtualPortInterface vpi = factory.Create("loop");
        if(vpi == null) {
            fail("factory.Create() failed.");
        }

        try {
            info("Name: " + vpi.getName());
            vpi.setName("NewName");
            info("Name: " + vpi.getName());
        }
        catch (Exception e) {
            info("Exception.");
            e.printStackTrace();
            return false;
        }

        String data = "The quick brown fox jumped over the lazy dog.";
        vpi.Send(data.getBytes(), data.length());
        byte buffer[] = new byte[data.length()];
        int rv = vpi.Recv(buffer, 1);
        String data2 = new String(buffer);
        info("rv = " + rv + "  data: " + new String(buffer));
        if(!data.equals(data2)) {
            info("data mismatch.");
            info("sent='" + data + "'");
            info("recv='" + data2 + "'");
            fail("");
        }
        return true;
    }


    public static void main(String [ ] args)
    {
        TestJVPI tj = new TestJVPI();
        tj.LoopbackTest();
    }
}
