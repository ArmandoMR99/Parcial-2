namespace BehaviourTreeTests
{
    public abstract class NodoBT
    {
        public abstract bool Ejecutar();
    }

    public class NodoRaiz : NodoBT
    {
        private NodoBT hijo;

        public NodoRaiz(NodoBT hijo)
        {
            if (hijo is NodoRaiz)
                throw new ArgumentException("Un NodoRaiz no puede tener otro NodoRaiz como hijo.");

            this.hijo = hijo;
        }

        public override bool Ejecutar() => hijo?.Ejecutar() ?? false;
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
                if (hijo.Ejecutar()) return true;

            return false;
        }
    }

    public class NodoSecuencia : NodoCompuesto
    {
        public override bool Ejecutar()
        {
            foreach (var hijo in hijos)
                if (!hijo.Ejecutar()) return false;

            return true;
        }
    }

    public abstract class NodoTarea : NodoBT { }

    public class NodoVerificarPar : NodoTarea
    {
        private int numero;

        public NodoVerificarPar(int num) => numero = num;

        public override bool Ejecutar() => numero % 2 == 0;
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
        public void Root_TieneUnUnicoHijo()
        {
            Assert.Throws<ArgumentException>(() =>
                new NodoRaiz(new NodoRaiz(new NodoVerificarPar(2))));
        }

        [Test]
        public void NodoCompuesto_NoPuedeTenerRootComoHijo()
        {
            var selector = new NodoSelector();
            Assert.Throws<ArgumentException>(() => selector.AgregarHijo(new NodoRaiz(new NodoVerificarPar(2))));
        }

        [Test]
        public void RootVacio_RetornaFalse()
        {
            var root = new NodoRaiz(null);
            var tree = new BehaviourTree(root);
            Assert.IsFalse(tree.Ejecutar());
        }

        [Test]
        public void SequenceConFalse_RetornaFalse()
        {
            var secuencia = new NodoSecuencia();
            secuencia.AgregarHijo(new NodoVerificarPar(3));
            Assert.IsFalse(secuencia.Ejecutar());
        }

        [Test]
        public void SelectorConTrue_RetornaTrue()
        {
            var selector = new NodoSelector();
            selector.AgregarHijo(new NodoVerificarPar(2));
            Assert.IsTrue(selector.Ejecutar());
        }

        [Test]
        public void NodoCompuestoDebeTenerAlMenosUnHijo()
        {
            var selector = new NodoSelector();
            Assert.IsFalse(selector.Ejecutar());
        }
    }
}



// Posdata no he dormido por este parcial, :) gracias profe
