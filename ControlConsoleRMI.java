
import java.rmi.Remote;
import java.rmi.RemoteException;

public interface ControlConsoleRMI extends Remote {
    void afficherTemperature(String piece, int temperature) throws RemoteException;
}
