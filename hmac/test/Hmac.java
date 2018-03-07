import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.KeyGenerator;
import javax.crypto.Mac;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;

public class Hmac {

	public static final String HASH_TYPE = "HmacSHA256";
	public static final String KEY = "tips";

	public static void main(String[] args) {
		// Generate secret key for HMAC-MD5
		KeyGenerator kg;
		try {
			
			 SecretKey secretKey = new SecretKeySpec(KEY.getBytes(), HASH_TYPE);
//			kg = KeyGenerator.getInstance("HmacSHA256");
//			SecretKey sk = kg.generateKey();

			// Get instance of Mac object implementing HMAC-MD5, and
			// initialize it with the above secret key
			Mac mac = Mac.getInstance(secretKey.getAlgorithm());
			mac.init(secretKey);
			byte[] result = mac.doFinal("message".getBytes());
			System.out.println(result.toString());
			
			StringBuffer sb = new StringBuffer(result.length * 2);
			for (int i = 0; i < result.length; i++) {
				int v = result[i] & 0xff;
				if (v < 16) {
					sb.append('0');
				}
				sb.append(Integer.toHexString(v));
			}
			
			System.out.println(sb.toString());
		} catch (NoSuchAlgorithmException | InvalidKeyException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
}
