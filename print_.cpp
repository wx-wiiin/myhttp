#include "print_.h"
void Print()
{
	#if 0
			int* root = (int*)pthread_getspecific(epoll_root);
			struct thread_list_node*h = head->next;
			h = head->next;
			
			while(h!=NULL)
			{
				cout<<"[ pthread_list.cpp 86 ]:thread id ["<<h->num
					<<"] using "<< 100*
					((double)(h->conned - h->disconned))/h->max_event
					<<"%   session_num:"<<h->session_num<<endl;
				for(struct session_list_node* t= h->session_head;t!=NULL;t=t->next)
					cout<<"          client ["<<t->client_fd<<"]----request_num:"<<t->request_num<<endl;

				h = h->next;
			}
			cout<<endl;
	#endif
}