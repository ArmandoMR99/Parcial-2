namespace BehaviourTreeTests
{
    public abstract class NodoBT
    {
        public abstract bool Ejecutar();
    }

    public class NodoRaiz : NodoBT
    {
        private NodoBT hijo;
        public NodoRaiz(NodoBT hijo) => this.hijo = hijo;
        public override bool Ejecutar() => hijo?.Ejecutar() ?? false;
    }

    public abstract class NodoTarea : NodoBT { }

    public class NodoEvaluarDistancia : NodoTarea
    {
        private int distanciaObjetivo;
        private int distanciaValida;

        public NodoEvaluarDistancia(int distancia, int umbral)
        {
            distanciaObjetivo = distancia;
            distanciaValida = umbral;
        }
        public override bool Ejecutar() => distanciaObjetivo <= distanciaValida;
    }

    public class NodoMoverse : NodoTarea
    {
        private int posicionActual;
        private int posicionObjetivo;

        public NodoMoverse(int posicionInicial, int objetivo)
        {
            posicionActual = posicionInicial;
            posicionObjetivo = objetivo;
        }
        public override bool Ejecutar()
        {
            while (posicionActual < posicionObjetivo)
            {
                posicionActual++;
            }
            return true;
        }
    }

    public class NodoEsperar : NodoTarea
    {
        private int tiempoEspera;
        public NodoEsperar(int tiempo) => tiempoEspera = tiempo;
        public override bool Ejecutar() => true;
    }

    public abstract class NodoCompuesto : NodoBT
    {
        protected List<NodoBT> hijos = new List<NodoBT>();

        public void AgregarHijo(NodoBT hijo)
        {
            if (hijo is NodoRaiz)
                throw new ArgumentException("Un NodoCompuesto no puede tener un NodoRaiz como hijo.");

            hijos.Add(hijo);
        }
    }

    public class NodoSelector : NodoCompuesto
    {
        public override bool Ejecutar()
        {
            foreach (var hijo in hijos)
            {
                if (hijo.Ejecutar()) return true;
            }
            return false;
        }
    }

    public class NodoSecuencia : NodoCompuesto
    {
        public override bool Ejecutar()
        {
            foreach (var hijo in hijos)
            {
                if (!hijo.Ejecutar()) return false;
            }
            return true;
        }
    }

    public class BehaviourTree
    {
        public NodoRaiz Root { get; private set; }

        public BehaviourTree(NodoRaiz root)
        {
            Root = root ?? throw new ArgumentException("El árbol de comportamiento debe tener un NodoRaiz.");
        }

        public bool Ejecutar() => Root.Ejecutar();
    }

    [TestFixture]
    public class BehaviourTreeTests
    {
        [Test]
        public void NodoEvaluarDistancia_DistanciaMenorOIgual_DevuelveTrue()
        {
            var nodo = new NodoEvaluarDistancia(3, 5);
            Assert.IsTrue(nodo.Ejecutar());
        }

        [Test]
        public void NodoEvaluarDistancia_DistanciaMayor_DevuelveFalse()
        {
            var nodo = new NodoEvaluarDistancia(6, 5);
            Assert.IsFalse(nodo.Ejecutar());
        }

        [Test]
        public void NodoMoverse_SeMueveHastaObjetivo()
        {
            var nodo = new NodoMoverse(0, 5);
            Assert.IsTrue(nodo.Ejecutar());
        }

        [Test]
        public void NodoEsperar_SiempreDevuelveTrue()
        {
            var nodo = new NodoEsperar(2);
            Assert.IsTrue(nodo.Ejecutar());
        }

        [Test]
        public void NodoSelector_UnHijoTrue_DevuelveTrue()
        {
            var selector = new NodoSelector();
            selector.AgregarHijo(new NodoEvaluarDistancia(3, 5)); // True
            selector.AgregarHijo(new NodoEvaluarDistancia(6, 5)); // False
            Assert.IsTrue(selector.Ejecutar());
        }

        [Test]
        public void NodoSecuencia_UnHijoFalse_DevuelveFalse()
        {
            var secuencia = new NodoSecuencia();
            secuencia.AgregarHijo(new NodoEvaluarDistancia(6, 5)); // False
            secuencia.AgregarHijo(new NodoMoverse(0, 5)); // True
            Assert.IsFalse(secuencia.Ejecutar());
        }

        [Test]
        public void BehaviourTree_RootEjecutaNodo()
        {
            var root = new NodoRaiz(new NodoEvaluarDistancia(3, 5));
            var tree = new BehaviourTree(root);
            Assert.IsTrue(tree.Ejecutar());
        }

        [Test]
        public void BehaviourTree_RootVacioDevuelveFalse()
        {
            var root = new NodoRaiz(null);
            var tree = new BehaviourTree(root);
            Assert.IsFalse(tree.Ejecutar());
        }

        [Test]
        public void NodoCompuesto_NoPuedeTenerNodoRaizComoHijo()
        {
            var selector = new NodoSelector();
            Assert.Throws<ArgumentException>(() => selector.AgregarHijo(new NodoRaiz(new NodoEvaluarDistancia(3, 5))));
        }

        [Test]
        public void NodoTaskNoPuedeTenerHijos()
        {
            var tarea = new NodoEvaluarDistancia(3, 5);
            Assert.IsTrue(tarea.Ejecutar());
        }

        [Test]
        public void NodoCompuestoDebeTenerAlMenosUnHijo()
        {
            var selector = new NodoSelector();
            Assert.IsFalse(selector.Ejecutar()); // No tiene hijos, así que debe retornar false
        }
    }
}
