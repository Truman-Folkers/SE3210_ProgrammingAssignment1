/* To compile this program:
 *   gcc CS3210_f2026_pa1_quad_sort.c -o CS3210_f2026_pa1_quad_sort
 * Change N and recompile to run on arrays of different sizes.
 *
 * For this assignment, you'll be implementing a simple, n-squared sort in
 * the LEG subset of the ARMv8 ISA.  To add a bit of complexity, we break the
 * array to be sorted into four sub-arrays which are sorted separately and
 * then merged in place.
 *
 * Regardless of how comfortable you may be with sorting, the presented
 * algorithm, or with how well you may comprehend this C implementation, we
 * want to stress here that you do not have to understand this algorithm!
 * You don't even have to try to understand it.  All that you have to do is,
 * essentially, be the compiler and translate this C code into LEG.  This is
 * not to discourage understanding.  Understanding is a good and useful
 * thing.  We are simply pointing out that you don't need to understand the
 * algorithm in order to implement your solution.  It's not rare that a route
 * of expedience is optimal.
 *
 * See the comments and code below for details on exactly what you must 
 * implement.
 *
 * Note that the C implementation given below is non-optimal.  Functions have
 * been factored in such a way as to force you to write a number of LEGv8
 * procedures and to use the stack!  You may not refactor the code to avoid
 * these things.  Of course, some limited refactoring is necessary due to the
 * different functionalities of C and LEG, but your refactoring should be
 * limited to transformations that you deem necessary to solve the problem
 * (restructuring branches or loops, for example).  You may not refactor in a
 * way that, e.g., eliminates procedures or reduces the need for stack
 * manipulation.
 *
 * All functions given below must be implemented as written except for main.
 * See the comment on main for details there.
 */

#define N 512

/* Returns zero if array a of n elements is in non-descending sorted order;
 * else returns non-zero.
 */
long check_sorted(unsigned long *a, long n)
{
  long i;

  for (i = 1; i < n; i++) {
    if (a[i - 1] > a[i]) {
      return 1;
    }
  }

  return 0;
}

/* Swaps the values a[i] and a[j].
 */
void swap(unsigned long *a, long i, long j)
{
  long tmp;

  tmp = a[i];
  a[i] = a[j];
  a[j] = tmp;
}

/* A helper for quad_sort_helper implementing the inner loop of the sort.  a
 * is an array of n elements.  i is an index against whose value all other
 * values are compared.
 */
void quad_sort_helper_helper(unsigned long *a, long n, long i)
{
  long j;

  for (j = 0; j < n; j++) {
    if (a[i] < a[j]) {
      swap(a, i, j);
    }
  }
}

/* Sorts array a of n elements into non-descending order.  This implements
 * the outer loop of an unnamed n-squared comparison sort.
 */
void quad_sort_helper(unsigned long *a, long n)
{
  long i;

  for (i = 0; i < n; i++) {
    quad_sort_helper_helper(a, n, i);
  }
}

/* Moves all elements in array a from last to (hole - 1) one position
 * backward.  The value originally in index last is lost.
 */
void quad_sort_shift(unsigned long *a, long last, long hole)
{
  long i;

  for (i = last; i != hole; i--) {
    a[i] = a[i - 1];
  }
}

/* Array a with n elements with elements [0,an) in non-descending order and
 * elements [an,n) in non-descending order.  The two sub-arrays are merged in
 * place leaving a in non-descending sorted order.
 */
void quad_sort_merge(unsigned long *a, long an, long n)
{
  long ai, bi;
  long i, val;
  
  ai = 0;
  bi = an;

  if (a[an - 1] > a[bi]) {
    while (ai < an && bi < n) {
      if (a[ai] <= a[bi]) {
        ai++;
      } else {
        val = a[bi];
        i = bi;

        quad_sort_shift(a, i, ai);

        a[ai] = val;

        ai++;
        an++;
        bi++;
      }
    }
  }
}

/* Breaks array a of n elements into 4 equal parts (if n is not a multiple of
 * 4, all extra elements modulo 4 are placed in the last part), sorts each of
 * those parts using an unnamed n-squared sort, and merges the parts
 * together, leaving a in non-decending sorted order.
 */
void quad_sort(unsigned long *a, long n)
{
  quad_sort_helper(a, n >> 2);
  quad_sort_helper(a + (n >> 2), n >> 2);
  quad_sort_helper(a + ((n >> 2) << 1), n >> 2);
  quad_sort_helper(a + ((n >> 2) + ((n >> 2 << 1))), n - ((n >> 2) + ((n >> 2) << 1)));

  quad_sort_merge(a, n >> 2, (n >> 2) << 1);
  quad_sort_merge(a, (n >> 2) << 1, (n >> 2) + ((n >> 2) << 1));
  quad_sort_merge(a, (n >> 2) + ((n >> 2) << 1), n);
}

//x19 = a
//x20 = n
//when you're writing code as comments to explain
//other code that's when you know you're cooked

/* Populates array a of n elements with the values 1 through n, inclusive, in
 * descending order.
 */
void fill(unsigned long *a, long n)
{
  int i;
  for (i = 0; i < n; i++) {
    a[i] = n - i;
  }
}

/* Programs in our emulator do not have an explicit main procedure.  Instead,
 * programs begin at the top of the assembly file and they terminate either
 * when they leave the bottom or when a HALT is issued.  So, in a sense, your
 * program is "in main" when it starts at the top of your file.
 *
 * Your "main" must do the following things in the given order:
 *
 *  * Create your array
 *  * Call fill to populate your array in descending order
 *  * Call check_sorted and halt if sorted
 *  * Call quad_sort to sort your array
 *  * Call check_sorted and halt if not sorted
 *  * Finally, unconditionally issue a HALT instruction.
*/
int main(int argc, char *argv[])
{
  long a[N];

  fill(a, N);

  if (!check_sorted(a, N)) {
    return -1; // You should use HALT here, but since your array is not
               // sorted, you should not actually halt.
  }
  
  quad_sort(a, N);

  if(check_sorted(a, N)) {
    return -2; // Similarly, use HALT, but again you should not actually halt.
  }
  
  return 0; // Issue an unconditional HALT here.
}