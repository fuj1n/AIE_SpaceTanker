namespace collision{
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
		return ((aw < ax || aw > ax) && (ah < ay || ah > by) && (bw < bx || bw > ax) && (bh < by || bh > ay));
	}
}