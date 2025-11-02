class Problem():

    size = 0
    var_count = 0
    def __init__(self, clauses: list):
        self.F = clauses

    def get_fct(self):
        return self.F
    
    def satisfy(self):
        for i in range(2**(Problem.size)):
            perm = bin(i)[2:].zfill(Problem.size)
            if self.check([int(i) for i in perm]):
                return True
        
        print("Insatisfaisable : " + str(self.get_fct()))
        return False

    def check(self, Fs: list):
        result = True
        # print("Trying : " + str(Fs))

        #pour chaque clause
        for count, clause in enumerate(self.get_fct()):
            clause_eval = False

            #pour chaque variable du pb
            for index in range(len(clause)):
                var = clause[index]

                #si elle est dans la clause
                if var > 0:
                    clause_eval = clause_eval or Fs[var - 1]
                elif var < 0:
                    clause_eval = clause_eval or not Fs[(-var) - 1]

                if clause_eval:
                    break
                    
            
            result = result and clause_eval

            if not result:
                # print("clause " + str(count) + " : " + str(bool(clause_eval)))
                return False
        
        print("Satisfied with: " + str(Fs))
        return bool(result)


Problem.size = 3
Problem.var_count = 3

# F = [(1, -3), (1,), (2, 3), (-3,)]

F = [(1, 2, 3), (-1, -2, -3), (1, -2, -3), (-1, 2, -3), (-1, -2, 3), (1, 2, -3), (1, -2, 3), (-1, 2, 3)]

P = Problem(F)

# sol = [1, 1, 0]
print(P.get_fct())
P.satisfy()
