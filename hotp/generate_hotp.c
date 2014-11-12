#include "hmac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// These are used to calculate the check-sum digits.
//0  1  2  3  4  5  6  7  8  9
static  int doubleDigits[] ={ 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };

// 0 1  2   3    4     5      6       7        8
static int DIGITS_POWER[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000};
/**
 * Calculates the checksum using the credit card algorithm.
 * This algorithm has the advantage that it detects any single
 * mistyped digit and any single transposition of
 * adjacent digits.
 *
 * @param num the number to calculate the checksum for
 * @param digits number of significant places in the number
 *
 * @return the checksum of num
 */
static int calcChecksum(long num, int digits) {
    bool doubleDigit = true;
    int     total = 0;
    while (0 < digits--) {
        int digit = (int) (num % 10);
        num /= 10;
        if (doubleDigit) {
            digit = doubleDigits[digit];
        }
        total += digit;
        doubleDigit = !doubleDigit;
    }
    int result = total % 10;
    if (result > 0) {
        result = 10 - result;
    }
    return result;
}

char   *myitoa(   int   value,   char   *str,   int   radix  )   
{   
    static   char   szMap[]   =   {   
        '0',   '1',   '2',   '3',   '4',   '5',   
        '6',   '7',   '8',   '9',   'a',   'b',   
        'c',   'd',   'e',   'f',   'g',   'h',   
        'i',   'j',   'k',   'l',   'm',   'n',   
        'o',   'p',   'q',   'r',   's',   't',   
        'u',   'v',   'w',   'x',   'y',   'z'   
    }; //   字符映射表   
    int   nCount   =   -1,   nIndex;   
    char   *pStr   =   str,   nTemp;   
    if   (   radix   >=   2   &&   radix   <=   36   )   
    {   //   限制radix必须在2到36之间   
        if   (   value   <   0   &&   radix   ==   10   )   
        {   //   如果是负数就在首位添加负号，并将字符串前移   
            *pStr++   =   '-';   
            value   =   -value;   //转为正数，   
        }   
        unsigned   int   nValue   =   *(unsigned*)&value;   
        do   { //   循环转换每一个数字，直到结束   
            pStr[   ++nCount   ]   =   szMap[   nValue   %   radix   ];   
            nValue   /=   radix;   
        }   while(   nValue   >   0   );   //   转换结束后字符串是翻的   
        nIndex   =   (   nCount   +   1   )   /   2;   //   计算出一半的长度   
        while(   nIndex--   >   0   )   {   //   将字符串的字符序翻转   
            nTemp   =   pStr[   nIndex   ];   
            pStr[   nIndex   ]   =   pStr[   nCount   -   nIndex   ];   
            pStr[   nCount   -   nIndex   ]   =   nTemp;   
        }   
    }   
    pStr[   nCount   +   1   ]   =   '\0';   //   置结束符   
    return   str;   
} 


/**
 * This method generates an OTP value for the given
 * set of parameters.
 *
 * @param secret       the shared secret
 * @param movingFactor the counter, time, or other value that
 *                     changes on a per use basis.
 * @param codeDigits   the number of digits in the OTP, not
 *                     including the checksum, if any.
 * @param addChecksum  a flag that indicates if a checksum digit
 *                     should be appended to the OTP.
 * @param truncationOffset the offset into the MAC result to
 *                     begin truncation.  If this value is out of
 *                     the range of 0 ... 15, then dynamic
 *                     truncation  will be used.
 *                     Dynamic truncation is when the last 4
 *                     bits of the last byte of the MAC are
 *                     used to determine the start offset.
 * @return A numeric String in base 10 that includes
 * {@link codeDigits} digits plus the optional checksum
 * digit if requested.
 */
char *generateOTP(uint8_t *secret , 
        int secretlen,
        uint8_t * text,
        int textlen,
        int codeDigits,
        bool addChecksum,
        int truncationOffset, char *result)
{
    int digits = addChecksum ? (codeDigits + 1) : codeDigits;
    uint8_t hash[20];
    int hashlen = sizeof(hash);
    // compute hmac hash
    hmac_sha1(secret, secretlen, text, textlen, hash, hashlen);

    // put selected bytes into result int
    int offset = hash[hashlen - 1] & 0xf;
    if ( (0<=truncationOffset) &&
            (truncationOffset<(hashlen-4)) ) {
        offset = truncationOffset;
    }
    int binary =
        ((hash[offset] & 0x7f) << 24)
        | ((hash[offset + 1] & 0xff) << 16)
        | ((hash[offset + 2] & 0xff) << 8)

        | (hash[offset + 3] & 0xff);

    int otp = binary % DIGITS_POWER[codeDigits];
    if (addChecksum) {
        otp =  (otp * 10) + calcChecksum(otp, codeDigits);
    }
    myitoa(otp, result, 10);
    while (strlen(result) < digits) {
        strcat(result, "0");
    }
    return result;
}


int main()
{
    char secret[] = "0x3132333435363738393031323334353637383930";
    char data[] = "hellotestdddfdasfdasfkjdashfkldhasfdkjasfhdaskjfhdasfa";
    char result[10] = {0};
    generateOTP(secret, strlen(secret),data, strlen(data), 6, 0, 0, result);
    printf("result:%s\n", result);
    return 0;
}
