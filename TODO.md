I want to seize galaxy...

Actually not )))

I'd like to develop db for limits control...

I want to do at client side something like:

LimdClient client;
if (!client.connect("127.0.0.1:13001")) {
    ...
}
client.setLimit("campaign1.budget", 10000);
client.setLimit("campaign1.budget_daily", 1000);
client.setLimit("campaign1.creative1.budget", 1000);
client.setLimit("campaign1.creative1.budget_daily", 100);

// try to hold budget
auto id = client.hold("campaign1.creative1.budget", 100);
if (id == 0) {
    // no budget
}
...
// commit budget spent
client.commit(id, 85); 
...
// release holded budget (not spent)
client.release(id);
...
// increase budget
client.inc("campaign1.creative1.budget", 10);
...
// decrease budget
client.dec("campaign1.creative1.budget", 10);
...
// get budget
client.get("campaign1.creative1.budget");
...
// set budget
client.set("campaign1.creative1.budget", 1000);
...
// get limit budget
client.getLimit("campaign1.creative1.budget");


There are to side: servers cluster and clients. Servers must works like master-master and hold all data. All limits must be divided between all clients.
