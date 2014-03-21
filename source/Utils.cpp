namespace Collision{

	/*
	Basic rectangular collision test
	ax = the X of the first rectangle
	ay = the Y of the first rectangle
	aw = the width of the first rectangle
	ah = the height of the first rectangle
	bx = the X of the second rectangle
	by = the Y of the second rectangle
	bw = the width of the second rectangle
	bh = the height of the second rectangle
	*/
	bool rect_intersects(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh){
		if (aw <= 0 || ah <= 0 || bw <= 0 || bh <= 0) {
		    return false;
		}
		//r = a t = b
		aw += ax;
		ah += ay;
		bw += bx;
		bh += by;
		//      overflow || intersect
		return ((aw < ax || aw > bx) && (ah < ay || ah > by) && (bw < bx || bw > ax) && (bh < by || bh > ay));
	}
}

namespace Random{
	int random(int nMin, int nMax){
		return nMin + (int)((double)rand() / (RAND_MAX+1) * (nMax-nMin+1));
	}
}