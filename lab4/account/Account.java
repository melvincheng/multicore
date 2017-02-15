package account;


//import java.lang.*;

public class Account {
    double amount;
    String  name;

    //constructor
  public Account(String nm,double amnt ) {
        amount=amnt;
        name=nm;
  }
  //functions
  synchronized  void depsite(double money){
      amount+=money;
      }

  synchronized  void withdraw(double money){
      amount-=money;
      }


/*
 * Activity 2
 * Adding a synchronized block causes a deadlock 
 * because if two different accounts are trying to transfer to each other,
 * both accounts will be locking their own resources while waiting for each other to free up their resources needed.
 */
  // synchronized  void transfer(Account ac,double mn){
  //   synchronized(ac){
  //     amount-=mn;
  //     ac.amount+=mn;
  //   }
  // }

/*
 * Activity 3
 * Locking the resources depending on the object hashcode prevents a deadlock 
 * because if both accounts try to transfer,
 * the locking order will be the same for both calls,
 * so that which ever transfer gets the first lock will be guaranteed to get the second lock 
 * as the other transfer will not have any locks
 */
  void transfer(Account ac,double mn){
    Account smallerAccount = this;
    Account largerAccount = ac;
    int thisCode = System.identityHashCode(this);
    int acCode = System.identityHashCode(ac);
    boolean order = thisCode < acCode;
    if(order){
      smallerAccount = this;
      largerAccount = ac;
    }else if(!order){
      smallerAccount = ac;
      largerAccount = this;
    }
    synchronized(smallerAccount){
      synchronized(largerAccount){
        amount-=mn;
        ac.amount+=mn;
      }
    }
  }

 synchronized void print(){
  System.out.println(name + "--"+amount);
  }

      }//end of class Acount
