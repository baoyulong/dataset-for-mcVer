pragma solidity >=0.4.18;

contract EtherAuction {

  // The address that deploys this auction and volunteers 1 eth as price.
  address public auctioneer;
  uint public auctionedEth = 0;

  uint public highestBid = 0;
  uint public secondHighestBid = 0;

  address public highestBidder;
  address public secondHighestBidder;

  uint public latestBidTime = 0;
  uint public auctionEndTime;

  mapping (address => uint) public balances;

  bool public auctionStarted = false;
  bool public auctionFinalized = false;

  constructor() public{
    auctioneer = msg.sender;
  }

  // The auctioneer has to call this function while supplying the 1th to start the auction
  function startAuction(uint block_timestamp) public payable{
    require(!auctionStarted);
    require(msg.sender == auctioneer);
    require(msg.value == 1);
    
    auctionedEth = msg.value;
    auctionStarted = true;
    auctionEndTime = block_timestamp + 5; // Ends 7 days after the deployment of the contract
  }

  //Anyone can bid by calling this function and supplying the corresponding eth
  function bid(uint block_timestamp) public payable {
    require(auctionStarted);
    require(block_timestamp < auctionEndTime);
    require(msg.sender != auctioneer);
    require(highestBidder != msg.sender); //If sender is already the highest bidder, reject it.

    address _newBidder = msg.sender;

    uint previousBid = balances[msg.sender];
    uint _newBid = msg.value + previousBid;

    require (_newBid  == highestBid + 2); //Each bid has to be 2 eth higher

    // The highest bidder is block_timestamp the second highest bidder
    secondHighestBid = highestBid;
    secondHighestBidder = highestBidder;

    highestBid = _newBid;
    highestBidder = msg.sender;

    latestBidTime = block_timestamp;
    //Update the bidder's balance so they can later withdraw any pending balance
    balances[msg.sender] = _newBid;

    //If there's less than an hour remaining and someone bids, extend end time.
    if(auctionEndTime - block_timestamp < 1){
      auctionEndTime += 1; // Each bid extends the auctionEndTime by 1 hour
    }
  }
  // Once the auction end has been reached, we distribute the ether.
  function finalizeAuction(uint block_timestamp) public {
    require (block_timestamp > auctionEndTime);
    require (!auctionFinalized);
    auctionFinalized = true;

    if(highestBidder == 0){
      //If no one bid at the auction, auctioneer can withdraw the funds.
      balances[auctioneer] = auctionedEth;
    }else{
      // Second highest bidder gets nothing, his latest bid is lost and sent to the auctioneer
      balances[secondHighestBidder] -= secondHighestBid;
      balances[auctioneer] += secondHighestBid;

      //Auctioneer gets the highest bid from the highest bidder.
      balances[highestBidder] -= highestBid;
      balances[auctioneer] += highestBid;

      //winner gets the 1eth being auctioned.
      balances[highestBidder] += auctionedEth;
      auctionedEth = 0;
    }
  }

  //Once the auction has finished, the bidders can withdraw the eth they put
  //Winner will withdraw the auctionedEth
  //Auctioneer will withdraw the highest bid from the winner
  //Second highest bidder will already have his balance at 0
  //The rest of the bidders get their money back
  function withdrawBalance() public{
    require (auctionFinalized);
    uint ethToWithdraw = balances[msg.sender];
    if(ethToWithdraw > 0){
      balances[msg.sender] = 0;
      msg.sender.transfer(ethToWithdraw);
    }

  }
  
}