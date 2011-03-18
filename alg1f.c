#include <setjmp.h>
#include "lanhep.h"

extern jmp_buf alg1_jmp_buf;

List alg1_sub_w(Term m1, Atomic ind, int c1, int i1, int c2, int i2, int i);


static void rename_ind(Term t, Label from, Label to)
	{
	if(is_list(t))
		{
		List l;
		l=t;
		while(!is_empty_list(l))
			{
			Term u;
			u=ListFirst(l);
			if(u==from)
				ChangeList(l,to);
			else
				rename_ind(u,from,to);
			l=ListTail(l);
			}
		return;
		}
	if(is_compound(t))
		{
		int i,ac;
		ac=CompoundArity(t);
		for(i=1;i<=ac;i++)
			{
			Term u;
			u=CompoundArgN(t,i);
			if(u==from)
				SetCompoundArg(t,i,to);
			else
				rename_ind(u,from,to);
			}
		}
	return;
	}


static void alg1_fix_w1(Term w)
	{
	List wi;
	List l1,l2,al;
	
	if(is_empty_list(CompoundArg2(w)))
		return;
	
	wi=NewList();
	l1=CompoundArg1(w);
	while(!is_empty_list(ListTail(l1)))
		{
		wi=AppendLast(wi,CompoundArg2(ListFirst(l1)));
		l1=ListTail(l1);
		}
	al=CompoundArg2(w);
	
	if(!is_empty_list(wi))
	while(!is_empty_list(al))
		{
		Term a1;
		List wia;
		a1=ListFirst(al);
		wia=NewList();
		l1=CompoundArg2(a1);
		while(!is_empty_list(l1))
			{
			wia=AppendLast(wia,CompoundArg2(ListFirst(l1)));
			l1=ListTail(l1);
			}
		if(ListLength(wi)!=ListLength(wia))
			{
			puts("Internal error a1fw");
			longjmp(alg1_jmp_buf,1);
			}
		l1=wi;
		l2=wia;
		while(!is_empty_list(l1))
			{
			rename_ind(a1,ListFirst(l2),ListFirst(l1));
			l1=ListTail(l1);
			l2=ListTail(l2);
			}
		FreeAtomic(wia);
		al=ListTail(al);
		}
	FreeAtomic(wi);		
	
	}
	

/*

static void alg1_fix_w1(Term w)
	{
	List wi;
	List l1,l2,al;
	wi=NewList();
	l1=CompoundArg1(w);
	while(!is_empty_list(ListTail(l1)))
		{
		wi=AppendLast(wi,CompoundArg2(ListFirst(l1)));
		l1=ListTail(l1);
		}
	al=CompoundArg2(w);
	
	if(!is_empty_list(wi))
	while(!is_empty_list(al))
		{
		Term a1;
		List wia;
		a1=ListFirst(al);
		wia=NewList();
		l1=CompoundArg2(a1);
		l2=wi;
		while(!is_empty_list(l1))
			{
			Term t1;
			t1=ListFirst(l1);
			wia=AppendLast(wia,CompoundArg2(t1));
			SetCompoundArg(t1,2,ListFirst(l2));
			l1=ListTail(l1);
			l2=ListTail(l2);
			}
		l1=CompoundArg1(a1);
		while(!is_empty_list(l1))
			{
			l2=CompoundArgN(ListFirst(l1),3);
			while(!is_empty_list(l2))
				{
				List l3;
				l3=CompoundArg1(ListFirst(l2));
				while(!is_empty_list(l3))
					{
					Term t1;
					int pos;
					t1=ListFirst(l3);
					pos=ListMember(wia,CompoundArg2(t1));
					if(pos)
						SetCompoundArg(t1,2,ListNth(wi,pos));
					l3=ListTail(l3);
					}
				l2=ListTail(l2);
				}
			l1=ListTail(l1);
			}
		al=ListTail(al);
		}
			
	
	}
	
*/

void alg1_fix_wild(Term a1)
	{
	List l1;
	l1=CompoundArg1(a1);
	while(!is_empty_list(l1))
		{
		List l2;
		l2=CompoundArgN(ListFirst(l1),3);
		while(!is_empty_list(l2))
			{
			if(CompoundName(ListFirst(l2))==OPR_WILD)
				alg1_fix_w1(ListFirst(l2));
			l2=ListTail(l2);
			}
		l1=ListTail(l1);
		}
	}

static List alg1_w_ind(Term m1, List wf)
	{
	List l1,l2,wi,wn;
	wi=wn=NewList();
	
		l2=CompoundArgN(m1,3);
		while(!is_empty_list(l2))
			{
			List l3;
			l3=CompoundArg1(ListFirst(l2));
			while(!is_empty_list(l3))
				{
				Atomic t1,t2;
				t1=ListFirst(l3);
				t2=CompoundArg2(t1);
				if(CompoundName(t1)==OPR_WILD && 
					!ListMember(wf,t2) && !ListMember(wi,t2))
						{
						wi=AppendLast(wi,t2);
						wn=AppendLast(wn,CompoundArgN(t1,3));
						}
				l3=ListTail(l3);
				}
			l2=ListTail(l2);
			}
	l1=wi;
	l2=wn;
	while(!is_empty_list(l1))
		{
		ChangeList(l1,MakeCompound2(OPR_DIV,ListFirst(l1),ListFirst(l2)));
		l1=ListTail(l1);
		l2=ListTail(l2);
		}
	RemoveList(wn);
	return wi;
	}

static List alg1_s_w_m_1(Term m1, Term by)
	{
	Atomic ind;
	int range;
	int i1,i2;
	int c1,c2;
	List l1;
	List ret;
	int i;
	
	i1=i2=0;
	c1=c2=0;
	i=1;	
	ind=CompoundArg1(by);
	range=IntegerValue(CompoundArg2(by));
	
	l1=CompoundArgN(m1,3);
	while(!is_empty_list(l1))
		{
		Term t1;
		t1=ListFirst(l1);
		if(CompoundName(t1)==OPR_WILD)
			{
			List l2;
			int no;
			no=1;
			l2=CompoundArg1(t1);
			while(!is_empty_list(l2))
				{
				Term t2;
				t2=ListFirst(l2);
				if(CompoundArg2(t2)==ind)
					{
					if(c1==0)
						{
						c1=i;
						i1=no;
						}
					else
						if(is_empty_list(c2))
							{
							c2=i;
							i2=no;
							}
						else
							{
							printf("Internal error (3rd index)\n");
							longjmp(alg1_jmp_buf,1);
							}
					}
				no++;
				l2=ListTail(l2);
				}
			}
		i++;
		l1=ListTail(l1);
		}
	
/*	
	printf("First:  %lx %d\n",c1,i1);
	printf("Second: %lx %d\n",c2,i2);
*/	
	
	ret=NewList();
	for(i=1;i<=range;i++)
		ret=ConcatList(ret,alg1_sub_w(m1,ind,c1,i1,c2,i2,i));
		
	return ret;
	}
	

static List alg1_s_w_m(Term m1, List sl)
	{
	List l1,ret;
	ret=NewList();
	ret=AppendLast(ret,m1);
	l1=sl;
	while(!is_empty_list(l1))
		{
		Term rrr;
		List l2;
		rrr=ret;
		ret=NewList();
		l2=rrr;
		while(!is_empty_list(l2))
			{
			ret=ConcatList(ret,alg1_s_w_m_1(ListFirst(l2),ListFirst(l1)));
			l2=ListTail(l2);
			}
		FreeAtomic(rrr);
		l1=ListTail(l1);
		}
	return ret;
	}

void alg1_sum_wild(Term a1)
	{
	List ww,l1,rl;
	List free_w;
	free_w=NewList();
	rl=NewList();
	
	l1=CompoundArg2(a1);
	while(!is_empty_list(l1))
		{
		Term t1;
		t1=ListFirst(l1);
		if(CompoundName(t1)==OPR_WILD)
			free_w=AppendFirst(free_w,CompoundArg2(t1));
		l1=ListTail(l1);
		}
	
	l1=CompoundArg1(a1);
	while(!is_empty_list(l1))
		{
		Term t1;
		t1=ListFirst(l1);			
		ww=alg1_w_ind(t1,free_w);
		if(is_empty_list(ww))
			{
			/*WriteTerm(t1); puts(":   pure");*/
			rl=AppendLast(rl,t1);
			}
		else	
			rl=ConcatList(rl,alg1_s_w_m(t1,ww));
		l1=ListTail(l1);
		}
	RemoveList(free_w);
	l1=ConsumeCompoundArg(a1,1);
	RemoveList(l1);
	SetCompoundArg(a1,1,rl);
	}

Term wdbg(Term t, Term ind)
	{
	t=CompoundArg1(t);
	t=GetAtomProperty(t,PROP_TYPE);
	t=CompoundArg1(t);
	printf("%d terms %d inds\n",ListLength(CompoundArg1(t)),
			ListLength(CompoundArg2(t)));
	return 0;
	}
	
	
