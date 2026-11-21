/*ATLACL.cpp*/
//#include <atlsecurity.h>
#include <security.h>
#include <iostream>
using namespace std;
void main()
{
	try{
		//the user accounts
		CSid sidBlake("Northwindtraders\\blake");
		CSid sidAdmin=Sids::Admins();
		CSid sidGuests=Sids::Guests();
		//Create the ACL, and populate with ACEs.
		//note the deny ACE is placed before the allow ACEs.
		CDacl dacl;
		dacl.AddDeniedAce(sidGuests, GENERIC_ALL);
		dacl.AddAllowedAce(sidBlake, GENERIC_READ);
		dacl.AddAllowedAce(sidAdmin, GENERIC_ALL);
		//Create the Security descriptor and attributes.
		CSecurityDesc sd;
		sd.SetDacl(dacl);
		CSecurityAttributes sa(sd);
		if(CreateDirectory("E:\\MyTestDir",&sa))
		cout<<"Directory created!"<<end1;
	} catch(CAtlException e)
		 {
			cerr << "Error, Application failed with error" 
				<< hex << (HRESULT)e << end1;
		}
}