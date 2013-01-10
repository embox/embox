package embox;

public class Hello {
	public static void main(final String args[]) {
		System.out.println("Hello, world!");
		if((null!=args) && (args.length!=0)) {
			Thread thread = new Thread(new Runnable() {
				public void run() {
					for(int i=0; i<args.length; i++) {
						System.out.println(">>> args[" + i + "]=" + args[i]);
					}
				}
			});
			thread.start();
			try {
				Thread.sleep(1000);
				thread.join();
			} catch (InterruptedException ie) {
				ie.printStackTrace();
			}
		}
	}
}
